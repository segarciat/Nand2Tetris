#include "HackWriter.h"

#include <regex>

HackWriter::HackWriter(std::string& sourceName)
	:m_ROMLines(0), m_RAMAddress(16)
{
	//Prepare to translate with parser, generator, and symbol table.
	m_pParser = new HackParser(sourceName + ".asm");
	m_pOutHackFile = new std::ofstream(sourceName + ".hack");
}

HackWriter::~HackWriter()
{
	delete m_pParser;
}

void HackWriter::BuildSymbols()
{
	//First pass -- store all labels in the program.
	while (m_pParser->HasMoreCommands())
	{
		m_pParser->Advance();
		if (m_pParser->GetCommandType() == HackParser::CommandType::L_COMMAND)
		{
			std::string symbol = m_pParser->Symbol();
			if (!m_SymbolTable.Contains(symbol))
				m_SymbolTable.AddEntry(symbol, m_ROMLines);
		}

		else if (m_pParser->GetCommandType() == HackParser::CommandType::C_COMMAND ||
			m_pParser->GetCommandType() == HackParser::CommandType::A_COMMAND)
			m_ROMLines++;
	}
}

void HackWriter::WriteHackCode()
{
	// First pass through file builds up the symbol table.
	BuildSymbols();

	// Gets parser to re-read the source assembly file.
	m_pParser->Reset();

	//Second pass -- handle variables and translate.
	while (m_pParser->HasMoreCommands())
	{
		m_pParser->Advance();

		// Write binary code for A command.
		if (m_pParser->GetCommandType() == HackParser::CommandType::A_COMMAND)
		{
			int decimalA;

			//Check A-Command is a constant...
			std::string symbol = m_pParser->Symbol();
			std::regex re("(\\d+)");
			if (std::regex_match(symbol, re))
				decimalA = std::stoi(symbol);
			else
			{ // ... or a variable.
				if (m_SymbolTable.Contains(symbol))
					decimalA = m_SymbolTable.GetAddress(symbol);
				else
				{
					decimalA = m_RAMAddress;
					m_SymbolTable.AddEntry(symbol, m_RAMAddress++);
				}
			}

			// Write the 16-bit constant to file.
			*m_pOutHackFile << To16BitBinary(decimalA) << "\n";
		}
		else if (m_pParser->GetCommandType() == HackParser::CommandType::C_COMMAND)
		{
			//Generate the code parsed from the assembly instructions.
			std::string compBits = m_CodeGenerator.Comp(m_pParser->Comp());
			std::string destBits = m_CodeGenerator.Dest(m_pParser->Dest());
			std::string jumpBits = m_CodeGenerator.Jump(m_pParser->Jump());

			//Append the 16-bit C-instruction to the hack file.
			std::string binaryC = "111" + compBits + destBits + jumpBits;
			*m_pOutHackFile << binaryC << "\n";
		}
	}
	m_pOutHackFile->close();
}

std::string HackWriter::To16BitBinary(int decimalA)
{
	std::string binaryA = "";

	//Compute binary representation of the input decimal value.
	for (int i = decimalA; i > 0; i /= 2)
		binaryA = std::to_string(i % 2) + binaryA;

	//Pad the binary string to make it 16-bits long.
	while (binaryA.size() < 16)
		binaryA = '0' + binaryA;
	return binaryA;
}