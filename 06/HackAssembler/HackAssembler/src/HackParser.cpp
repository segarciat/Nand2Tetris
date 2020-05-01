#include "HackParser.h"
#include <iostream>
#include <fstream>
#include <string>
#include <regex>

HackParser::HackParser(const std::string& filename)
{
	m_pInFile = new std::ifstream(filename);
	if (!m_pInFile->is_open())
		std::cout << "Unable to open file " << filename << std::endl;
}

HackParser::~HackParser()
{
	m_pInFile->close();
	delete m_pInFile;
}

bool HackParser::HasMoreCommands()
{
	// Proceed if there are no comments or blank spaces.
	while (m_pInFile->peek() != -1)
	{
		char c = m_pInFile->peek();

		// Ignore spaces.
		if (isspace(c))
			m_pInFile->get();
		// Ignore the line if there's a space.
		else if (c == '/')
			std::getline(*m_pInFile, m_CurrentCommand);
		// Since it's neither, it must be a valid character.
		else
			return true;
	}
	return false;
}

// Moves to next command. Only meant to be called if hasMoreCommands was called.
void HackParser::Advance()
{
	// Read next line of input and store it.
	std::getline(*m_pInFile, m_CurrentCommand);
	// Remove beginning and trailign spaces, as well as comments.
	CleanLine();
}

void HackParser::CleanLine()
{
	//Remove comments.
	size_t i = m_CurrentCommand.find("//");
	if (i != -1)
		m_CurrentCommand.erase(i, std::string::npos);

	//Removing leading and trailing spaces.
	m_CurrentCommand = std::regex_replace(m_CurrentCommand, std::regex("^(\\s+)|(\\s+)$"), "");
}

void HackParser::Reset()
{
	m_pInFile->seekg(0);
}

HackParser::CommandType HackParser::GetCommandType() const
{
	// Command starts with a @ are addressing commands.
	std::regex re ("(@)(.+)");
	if (std::regex_match(m_CurrentCommand, re))
		return CommandType::A_COMMAND;

	// Commands with either an equal sign are computations; those with ';' are jump commands.
	re.assign("(.+)[=;](.+)", std::regex::ECMAScript);
	if (std::regex_match(m_CurrentCommand, re))
		return CommandType::C_COMMAND;


	// Commands that start with ( and end with a ) are labels.
	re.assign("[\(](.+)[\)]", std::regex::ECMAScript);
	if (std::regex_match(m_CurrentCommand, re))
		return CommandType::L_COMMAND;

	return CommandType::NO_COMMAND;
}

std::string HackParser::Symbol() const
{
	// Get only the 
	if (m_CurrentCommand.find("@") != -1)
		return m_CurrentCommand.substr(1, std::string::npos);
	else
		return m_CurrentCommand.substr(1, m_CurrentCommand.size() - 2);
}

std::string HackParser::Dest() const
{
	// Match a sequence of letters only if followed by =
	size_t i = m_CurrentCommand.find("=");
	if (i != -1)
		return m_CurrentCommand.substr(0, i);
	else
		return "";
}

std::string HackParser::Comp() const
{
	size_t left = m_CurrentCommand.find("=");
	size_t right = m_CurrentCommand.find(";");
	if (left == -1) 
		return m_CurrentCommand.substr(0, right);

	if (right == -1)
		return m_CurrentCommand.substr(left + 1, std::string::npos);
	return m_CurrentCommand.substr(left + 1, right - left);
}

std::string HackParser::Jump() const
{
	size_t i = m_CurrentCommand.find(";");
	if (i != -1)
		return m_CurrentCommand.substr(i + 1, std::string::npos);
	else
		return "";
}