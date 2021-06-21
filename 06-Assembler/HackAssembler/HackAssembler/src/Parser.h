#pragma once
#include <string>
#include <fstream>

/*
* Reads Hack assembly source file and parses its mnemonics.
* Ignores white space and comments.
*/
class Parser 
{
private:
	// File stream for input Hack assembly source file
	std::ifstream m_Ifs;
	std::string m_CurrentCommand;
public:
	enum class CType { A_COMMAND, C_COMMAND, L_COMMAND, NO_COMMAND };
	explicit Parser(const std::string& name);
	~Parser();

	// Checks if there are more Hack commands
	bool HasMoreCommands();

	// Sets the current command
	void Advance();
	void Restart();

	CType CommandType() const;

	// A-instruction symbol, which could be a constant or a label
	std::string Symbol() const;

	// C-instruction mnemonic components
	std::string Dest() const;
	std::string Comp() const;
	std::string Jump() const;
};

