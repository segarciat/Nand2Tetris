#pragma once
#include <fstream>
#include <string>
#include <unordered_map>

/*
* The CodeWriter writes Hack assembly to a file from the given
* VM commands that are passed to it. It stops writing when
* it ceases to exist in memory (goes out of scope, for example).
*/

extern const std::string g_TARGET_EXT;
extern const std::string g_SRC_EXT;

class CodeWriter 
{
private:
	// Single output Hack assembly file
	std::ofstream m_Ofs;
	// Name of VM file currently being translated to assembly
	std::string m_CurrentFile;
	// Used to ensure label uniqueness (disambiguity)
	int m_LabelCount;
	static const std::unordered_map<std::string, std::string> s_CmdMap;
	static const std::unordered_map<std::string, std::string> s_SgmtMap;
public:
	explicit CodeWriter(const std::string& name);
	~CodeWriter();
	// Gets ready to translate new VM file
	void SetFileName(const std::string& name);

	// Write assembly output corresponding to given command
	void WriteArithmetic(const std::string& name);
	void WritePushPop(const std::string& command, const std::string& segment, int index);
};