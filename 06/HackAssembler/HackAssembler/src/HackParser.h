#pragma once
#include <iostream>
#include <fstream>

/*
	Purpose: The HackParser encapsulates access to the input code in an assembly
	file written for the Hack system. It reads an assembly language
	command, parses it, and provides convenient access to the command's
	components (fields and symbols). In addition, it removes all white
	space and comments.
*/

class HackParser
{
public:
	enum class CommandType
	{
		NO_COMMAND = 0, A_COMMAND, C_COMMAND, L_COMMAND
	};

	// Opens input file/stream.
	HackParser(const std::string& filename);

	// Destructor -- frees file object memory and closes file.
	~HackParser();
	
	bool HasMoreCommands();
	void Advance();

	//Set file pointer to beginning of file.
	void Reset();
	CommandType GetCommandType() const;
	std::string Symbol() const;
	std::string Dest() const;
	std::string Comp() const;
	std::string Jump() const;


private:
	// 
	std::ifstream* m_pInFile;
	std::string m_CurrentCommand;

	//Remove leading and training spaces, and comments.
	void CleanLine();
};