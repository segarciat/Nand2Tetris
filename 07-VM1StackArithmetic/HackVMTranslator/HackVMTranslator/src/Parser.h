#pragma once
#include <fstream>
#include <string>
#include <unordered_map>
#include <filesystem>

extern const std::string g_SRC_EXT;

namespace HackVM {
	enum class CType {
		C_ARITHMETIC,
		C_PUSH,
		C_POP,
		C_LABEL,
		C_GOTO,
		C_IF,
		C_FUNCTION,
		C_RETURN,
		C_CALL,
		C_INVALID
	};
}

// Wrapper for the input VM file that is to be parsed
static struct VMFile
{
	std::ifstream ifs;
	std::string name;
};

class Parser
{
private:
	VMFile m_VMFile;
	// Current command parsed from file stream
	std::string m_Command;
	// First argument of current command (if any)
	std::string m_Arg1;
	// Second argument of current command (if any)
	int m_Arg2;
	static const std::unordered_map<std::string, HackVM::CType> s_CmdMap;
public:
	explicit Parser(const std::filesystem::path& name);
	~Parser();

	// Checks if there are more VM commands
	bool HasMoreCommands();

	// Parses next command, ignoring white space and comments
	void Advance();

	HackVM::CType CommandType() const;

	// Command arguments (if any)
	std::string Arg1() const { return m_Arg1; }
	int Arg2() const { return m_Arg2; }
};