#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include "VMParser.h"

VMParser::VMParser(const std::string& filename) :
	m_CurrentCommand("")
{
	m_pFile = new std::ifstream(filename);
	if (!m_pFile->is_open())
		std::cout << "Unable to open " << filename << std::endl;
}

bool VMParser::HasMoreCommands()
{
	while (!m_pFile->eof() && m_pFile->peek() == '/')
		getline(*m_pFile, m_CurrentCommand);
	return !m_pFile->eof();
}

void VMParser::Advance()
{
	getline(*m_pFile, m_CurrentCommand);
	CleanLine();
}

void VMParser::CleanLine()
{
	//Remove comments.
	size_t i = m_CurrentCommand.find("//");
	if (i != -1)
		m_CurrentCommand.erase(i, std::string::npos);

	//Removing leading and trailing spaces.
	m_CurrentCommand = std::regex_replace(m_CurrentCommand, std::regex("^(\\s+)|(\\s+)$"), "");
}

VMParser::CommandType VMParser::GetCommandType()
{
	if (m_ArithCommands.find(m_CurrentCommand) != m_ArithCommands.end())
		return CommandType::C_ARITHMETIC;
	else if (m_CurrentCommand.find("push") != -1)
		return CommandType::C_PUSH;
	else if (m_CurrentCommand.find("pop") != -1)
		return CommandType::C_POP;
	else if (m_CurrentCommand.find("if") != -1)
		return CommandType::C_IF;
	else if (m_CurrentCommand.find("goto") != -1)
		return CommandType::C_GOTO;
	else if (m_CurrentCommand.find("label") != -1)
		return CommandType::C_LABEL;
	else if (m_CurrentCommand.find("call") != -1)
		return CommandType::C_CALL;
	else if (m_CurrentCommand.find("function") != -1)
		return CommandType::C_FUNCTION;
	else if (m_CurrentCommand.find("return") != -1)
		return CommandType::C_RETURN;
	return CommandType::NO_COMMAND;
}

//bool VMParser::IsArithmetic()
//{
//	for (int i = 0; i < 9; i++)
//		if (m_CurrentCommand == ARITH_COMMANDS[i])
//			return true;
//	return false;
//}

std::string VMParser::Arg1()
{
	std::stringstream ss(m_CurrentCommand);
	std::string temp;
	ss >> temp;
	// return the command itself, i.e., add, sub, etc
	if (m_ArithCommands.find(m_CurrentCommand) != m_ArithCommands.end())
		return temp;
	else
	{ // for non-arithmetic, like push, return the first argument, i.e., constant
		ss >> temp;
		return temp;
	}
}

int VMParser::Arg2()
{
	size_t lastBlank = m_CurrentCommand.find_last_of(" ");
	return stoi(m_CurrentCommand.substr(lastBlank + 1, std::string::npos));
}