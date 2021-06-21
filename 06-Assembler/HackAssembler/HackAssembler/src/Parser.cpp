#include "Parser.h"
#include <sstream>
#include <iostream>

Parser::Parser(const std::string& name)
{
	m_Ifs.open(name);
	if (!m_Ifs)
	{
		std::stringstream ss;
		ss << "Failed to open: " << name;
		throw std::ifstream::failure(ss.str());
	}
}

Parser::~Parser()
{
	m_Ifs.close();
}

void Parser::Restart()
{
	m_CurrentCommand = "";
	m_Ifs.seekg(0);
}

/*
* Ignores blanks and lines with "//", which refer to comments
*/
bool Parser::HasMoreCommands()
{
	char c;
	while ((c = m_Ifs.peek()) != EOF) {
		if (isspace(c))
			m_Ifs.get();
		else if (c == '/' && (c = m_Ifs.get()) == '/')
			m_Ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		else
			return true;
	}
	return false;
}

/* 
* Reads next command and makes it the current command.
* Ignores spacces and inline-comments
* 
* Note: Should only be called if HasMoreCommands() is true, but does not check for it.
*/
void Parser::Advance()
{
	std::getline(m_Ifs, m_CurrentCommand);
	std::stringstream iss{ m_CurrentCommand };
	std::stringstream oss;
	char c;
	while (iss >> c && c != '/')
		oss << c;
	oss >> m_CurrentCommand; // HasMoreCommands takes care of trailing spaces.
}

/* 
* Returns the command type associated with a m_CurrentCommand.
* NO_COMMAND is considered an invalid command.
* 
* Example:	@23		-> COMMAND
*			@sum	-> COMMNAD
*			(LOOP)	-> L_COMMAND (pseudo-command)
*			D=D|A	-> C_COMMAND
*			0;JMP	-> C_COMMAND
*/
Parser::CType Parser::CommandType() const
{
	if (m_CurrentCommand[0] == '@')
		return Parser::CType::A_COMMAND;
	else if (m_CurrentCommand[0] == '(')
		return Parser::CType::L_COMMAND;
	else if (m_CurrentCommand.find(';') != std::string::npos || m_CurrentCommand.find('=') != std::string::npos)
		return Parser::CType::C_COMMAND;
	return Parser::CType::NO_COMMAND;
}

/*
* Returns decimal constant or label of current command @Xxx or (Xxx)
* Note:	Should only be called if CommandType() is A_COMMAND or L_COMMAND
* 
* Example:	"@23"		-> "23"
*			"@sum"		-> "sum"
*			"(INIT)"	-> "INIT"
*/
std::string Parser::Symbol() const
{
	int len = m_CurrentCommand.size() - 1 - (m_CurrentCommand[0] == '(');
	return m_CurrentCommand.substr(1, len);
}

/*
* Returns destination portion of a C-instruction mnemonic
* 
* Example:	"D=M+1" -> "D"
*			"AD=1"	-> "AD"
*			"0;JMP" -> ""
*/
std::string Parser::Dest() const
{
	size_t len = m_CurrentCommand.find('=');
	return (len != std::string::npos) ? m_CurrentCommand.substr(0, len) : "";
}

/*
* Returns compute portion of a C-instruction mnemonic
*
* Example:	"D=M+1" -> "M+1"
*			"AD=1"	-> "1"
*			"0;JMP"	-> "0"
*/
std::string Parser::Comp() const
{
	size_t offset = m_CurrentCommand.find('=');
	offset = (offset == std::string::npos) ? 0 : offset + 1;
	size_t cutoff = m_CurrentCommand.find(';');
	size_t len = (cutoff == std::string::npos) ? cutoff: cutoff - offset;
	return m_CurrentCommand.substr(offset, len);
}

/*
* Returns jump portion of a C-instruction mnemonic
*
* Example:	"D=M+1" -> ""
*			"0;JMP"	-> "JMP"
*/
std::string Parser::Jump() const
{
	size_t offset = m_CurrentCommand.find(';');
	return (offset != std::string::npos) ? m_CurrentCommand.substr(offset + 1) : "";
}