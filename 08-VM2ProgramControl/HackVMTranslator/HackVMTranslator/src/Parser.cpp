#include <sstream>
#include "Parser.h"
#include "InvalidCommand.h"

const std::unordered_map<std::string, HackVM::CType> Parser::s_CmdMap = {
	{"add", HackVM::CType::C_ARITHMETIC},
	{"sub", HackVM::CType::C_ARITHMETIC},
	{"neg", HackVM::CType::C_ARITHMETIC},
	{"eq", HackVM::CType::C_ARITHMETIC},
	{"gt", HackVM::CType::C_ARITHMETIC},
	{"lt", HackVM::CType::C_ARITHMETIC},
	{"and", HackVM::CType::C_ARITHMETIC},
	{"or", HackVM::CType::C_ARITHMETIC},
	{"not", HackVM::CType::C_ARITHMETIC},
	{"push", HackVM::CType::C_PUSH},
	{"pop", HackVM::CType::C_POP},
	{"label", HackVM::CType::C_LABEL},
	{"goto", HackVM::CType::C_GOTO},
	{"if-goto", HackVM::CType::C_IF},
	{"function", HackVM::CType::C_FUNCTION},
	{"return", HackVM::CType::C_RETURN},
	{"call", HackVM::CType::C_CALL}
};

Parser::Parser(const std::filesystem::path& fp)
	:m_Command{ "" }, m_Arg1{ "" }, m_Arg2{ 0 }
{
	m_VMFile.name = fp.stem().string();
	m_VMFile.ifs.open(fp);
	if (!m_VMFile.ifs)
	{
		std::stringstream ss;
		ss << "Problem encountered while opening \"" << m_VMFile.name << "\"";
		throw std::ifstream::failure(ss.str());
	}
}

Parser::~Parser()
{
	if (m_VMFile.ifs.is_open())
		m_VMFile.ifs.close();
}

bool Parser::HasMoreCommands()
{
	char c;
	while ((c = m_VMFile.ifs.peek()) != EOF)
	{
		if (isspace(c))											// Ignore white space
			m_VMFile.ifs.get();
		else if (c == '/' && (c = m_VMFile.ifs.peek()) == '/')	// Ignore comments
			m_VMFile.ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		else
			return true;
	}
	return false;
}

void Parser::Advance()
{
	std::string line;
	std::getline(m_VMFile.ifs, line);			// Read line and get ready to parse
	std::stringstream ss{ line };
	HackVM::CType ctype;
	ss >> m_Command;
	if (ss >> m_Arg1 && m_Arg1[0] != '/')		// Argument (non-comment)
		ss >> m_Arg2;							// May have second argument
	else if ((ctype = CommandType()) == HackVM::CType::C_ARITHMETIC)
		m_Arg1 = m_Command;
	else if (ctype == HackVM::CType::C_RETURN)	// Return takes no arguments
		return;
	else
		throw HackVM::InvalidCommand(m_VMFile.name);
}

HackVM::CType Parser::CommandType() const
{
	auto it = s_CmdMap.find(m_Command);
	if (it == s_CmdMap.end())
		return HackVM::CType::C_INVALID;
	else return it->second;
}