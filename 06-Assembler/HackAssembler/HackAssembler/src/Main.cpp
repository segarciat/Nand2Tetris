#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <sstream>
#include "Parser.h"
#include "Code.h"
#include "SymbolTable.h"
#include "CommandError.h"

namespace fs = std::filesystem;

// Number of bits in each Hack machine word
size_t g_WORD_SIZE = 16;
size_t BASE_VAR_ADDRESS = 16;

// Convert n to binary as a string that is g_WORD_SIZE long
std::string toBinary(int n);

/*
* Assemble Hack machine code instructions from Hack assembly files
* 
* Input:	Hack assembly files (.asm extension) as command-line arguments
* Output:	Hack machine code files (.hack extension) in current directory
*/
int main(int argc, char** argv)
{
	if (argc == 1) 
	{
		std::cerr << "HackAssembler: Compile Hack assembly files with .hack extension to binary.";
		std::cerr << std::endl << "Usage: [FILE]..." << std::endl;
		return EXIT_FAILURE;
	}
	while (--argc > 0)
	{
		fs::path f{ *++argv };
		std::string instruction;
		size_t instruction_no;
		if (f.extension() != ".asm")
		{
			std::cerr << "HackAssembler: Invalid file extension in " << f;
			std::cerr << ". Expected \".asm\"" << std::endl;
			return EXIT_FAILURE;
		}
		try
		{
			std::cout << "Parsing " << f.string() << std::endl;
			Parser parser{ f.string() };
			SymbolTable st;
			instruction_no = 0;
			// First pass to populate symbol table with labels
			while (parser.HasMoreCommands())
			{
				parser.Advance();
				Parser::CType c_type = parser.CommandType();
				if (c_type == Parser::CType::A_COMMAND || c_type == Parser::CType::C_COMMAND)
					instruction_no++;
				else if (c_type == Parser::CType::L_COMMAND)
					st.AddEntry(parser.Symbol(), instruction_no);
			}
			// Second pass to handle variables and translate file
			std::ofstream ofs{ f.replace_extension("hack").filename().string() };
			if (!ofs)
				throw std::ofstream::failure("Problem while creating " + f.filename().string());
			parser.Restart();
			instruction_no = 0;
			int next_var_address = BASE_VAR_ADDRESS;
			while (parser.HasMoreCommands())
			{
				instruction = "";
				parser.Advance();
				Parser::CType c_type = parser.CommandType();
				if (c_type == Parser::CType::A_COMMAND) {
					std::string symbol = parser.Symbol();
					int address;
					if (!symbol.empty() && symbol.find_first_not_of("0123456789") == std::string::npos)
						address = std::stoi(symbol);
					else if (!isdigit(symbol[0])) {
						if (!st.Contains(symbol))
							st.AddEntry(symbol, next_var_address++);
						address = st.GetAddress(symbol);
					}
					else
						throw Hack::CommandError();
					instruction = toBinary(address);
				}
				else if (c_type == Parser::CType::C_COMMAND)
					// All C commands have three leftmost 1 bits
					instruction = "111" +
						Code::Comp(parser.Comp()) +
						Code::Dest(parser.Dest()) +
						Code::Jump(parser.Jump());
				else if (parser.CommandType() == Parser::CType::L_COMMAND)
					continue;
				if (instruction.size() != g_WORD_SIZE)
					throw Hack::CommandError();
				ofs << instruction << std::endl;
				instruction_no++;
			}
			ofs.close();
		}
		catch (const std::exception& e)
		{
			std::cerr << f.filename().string() << " line " << instruction_no << ": ";
			std::cerr << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	return EXIT_SUCCESS;
}

static std::string toBinary(int n)
{
	std::string address;
	for (; n > 0; n /= 2)
		address = std::to_string(n % 2) + address;
	while (address.length() < g_WORD_SIZE)
		address = "0" + address;
	return address;
}