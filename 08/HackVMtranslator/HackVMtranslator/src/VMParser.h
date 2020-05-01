#pragma once
#include <iostream>
#include <fstream>
#include <set>

/**
	Purpose: Handles the parsing of a single .vm file, and encapsulates
	access to the input code. It reads VM commands, parases them, and
	provides convenient access to their components. In addition, it
	removes all white space and comments.
*/

class VMParser
{
public:
	enum class CommandType {
		C_ARITHMETIC = 0,
		C_PUSH, C_POP,
		C_LABEL,
		C_GOTO,
		C_IF,
		C_FUNCTION,
		C_RETURN,
		C_CALL,
		NO_COMMAND
	};

	// Creates input file object to get ready for reading.
	VMParser(const std::string& filename);

	// Checks that there are more commands in the input.
	bool HasMoreCommands();

	// Reads next commands from input and makes it the current command.
	void Advance();

	// Returns the type of the current VM command.
	CommandType GetCommandType();

	// Returns the first argument of the current command (except C_RETURN).
	// If the command is arithmetic (i.e., add, sub), returns the command itself.
	std::string Arg1();

	// Returns the second argument of the current C_PUSH, C_POP, C_FUNCTION or C_CALL.
	int Arg2();

private:
	// Arithmetic command names.
	const std::set<std::string> m_ArithCommands = { "add", "sub", "neg",
											"eq", "gt", "lt",
											"and", "or", "not" };

	// Input VM file to parse.
	std::ifstream* m_pFile;

	// The latest command read after a call to Advance().
	std::string m_CurrentCommand;
	

	//bool IsArithmetic();

	// Removes comments and white space from line.
	void CleanLine();
	
};