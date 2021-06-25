#include <sstream>
#include "CodeWriter.h"
#include "InvalidCommand.h"

const std::unordered_map<std::string, std::string> CodeWriter::s_CmdMap = {
	{"add", "+"},
	{"sub", "-"},
	{"and", "&"},
	{"or", "|" },
	{"eq", "JEQ"},
	{"lt", "JLT"},
	{"gt", "JGT"},
	{"not", "!"},
	{"neg", "-"}
};

const std::unordered_map<std::string, std::string> CodeWriter::s_SgmtMap = {
	{"local", "LCL"},
	{"argument", "ARG"},
	{"this", "THIS"},
	{"that", "THAT"},
	{"pointer", "THIS"},
	{"temp", "R5"},
	{"constant", "constant"},
	{"static", "static"}
};

CodeWriter::CodeWriter(const std::string& name):
	m_LabelCount(0)
{
	m_Ofs.open(name + g_TARGET_EXT);
	if (!m_Ofs)
	{
		std::stringstream ss;
		ss << "Problem encountered while creating " << name;
		throw std::ofstream::failure(ss.str());
	}
}

CodeWriter::~CodeWriter()
{
	if (m_Ofs.is_open())
		m_Ofs.close();
}

void CodeWriter::SetFileName(const std::string& name)
{

	m_CurrentFile = name;
	m_LabelCount = 0;		// Reset label count for new file
}

/*
* Writes commands that perform binary unary operations.
*/
void CodeWriter::WriteArithmetic(const std::string& command)
{
	auto it = s_CmdMap.find(command);
	if (it == s_CmdMap.end())
		throw HackVM::InvalidCommand(m_CurrentFile + g_SRC_EXT);
	if (command == "add" || command == "sub" || command == "and" || command == "or") {
		// Pop first value from stack, and decrease stack pointer
		m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\n";
		// Replace next value by resultant of operation
		m_Ofs << "A=M\nM=M" << it->second << "D\n@SP\nM=M+1\n";
	}
	else if (command == "eq" || command == "lt" || command == "gt")
	{
		// Pop top two values and take their difference.
		m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n";
		// Jump to TRUE label if JEQ ("eq"), JLT ("lt"), or JGT ("gt")
		m_Ofs << "@_" << ++m_LabelCount << UniqueLabel("TRUE") << "\nD;" << it->second;
		// Assign 0 if false and jump to end
		m_Ofs << "\nD=0\n@_" << m_LabelCount << UniqueLabel("ENDTRUE") << "\n0;JMP\n";
		// Assign -1 if true
		m_Ofs << "(_" << m_LabelCount << UniqueLabel("TRUE") << ")\nD=-1\n";
		// Push 0 or -1 to top of stack
		m_Ofs << "(_" << m_LabelCount << UniqueLabel("ENDTRUE") << ")\n";
		m_Ofs << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
	}
	else if (command == "not" || command == "neg")
		m_Ofs << "@SP\nM=M-1\nA=M\nM=" << it->second << "M\n@SP\nM=M+1\n";
}

/*
* Writes commands that effect a push (pop) segment index operation.
*/
void CodeWriter::WritePushPop(const std::string& command, const std::string& segment, int index)
{
	auto it = s_SgmtMap.find(segment);
	if (it == s_SgmtMap.end() || (command != "push" && command != "pop"))
		throw HackVM::InvalidCommand(m_CurrentFile + g_SRC_EXT);
	if (command == "push")
	{
		// Save value of static variable
		if (segment == "static")
			m_Ofs << "@" << m_CurrentFile << "." << index << "\nD=M\n";
		else
		{
			// Push the index value (if constant) or use as segment offset
			m_Ofs << "@" << index << "\nD=A\n";
			if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
				m_Ofs << "@" << it->second << "\nA=M+D\nD=M\n";
			else if (segment == "pointer" || segment == "temp")
				m_Ofs << "@" << it->second << "\nA=A+D\nD=M\n";
		}
		// Push D to stack
		m_Ofs << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
	}
	else if (command == "pop")
	{
		if (segment == "constant")
			return;
		else if (segment == "static")
		{
			// Get value at top of stack
			m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n";
			// Assign value to the static variable at given index
			m_Ofs << "@" << m_CurrentFile << "." << index << "\nM=D\n";
			return;
		}
		// Calculate base segment address + offset
		m_Ofs << "@" << index << "\nD=A\n@" << it->second << "\n";
		if (segment == "local" || segment == "argument" || segment == "this" ||
			segment == "that") 
			m_Ofs << "D=M+D\n";
		else if (segment == "pointer" || segment == "temp")
			m_Ofs << "D=A+D\n@R13\nM=D\n";
		// Store value popped from stack at computed address
		m_Ofs << "@R13\nM=D\n@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n";
	}
}

const std::string CodeWriter::UniqueLabel(const std::string& label)
{	// (functionName$label:labelCount)
	return m_CurrentFunction + "$" + label;
}