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

void CodeWriter::WriteArithmetic(const std::string& command)
{
	auto it = s_CmdMap.find(command);
	if (it == s_CmdMap.end())
		throw HackVM::InvalidCommand(m_CurrentFile + g_SRC_EXT);
	if (command == "add" || command == "sub" || command == "and" || command == "or") {
		m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\n";
		m_Ofs << "M=M" << it->second << "D\n@SP\nM=M+1\n";
	}
	else if (command == "eq" || command == "lt" || command == "gt")
	{
		std::string label = m_CurrentFile + std::to_string(m_LabelCount);
		m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@TRUE:" << label;
		m_Ofs << "\nD;" << it->second << "\nD=0\n@ENDTRUE:" << label << "\n0;JMP\n";
		m_Ofs << "(TRUE:" << label << ")\nD=-1\n(ENDTRUE:" << label << ")\n";
		m_Ofs << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		m_LabelCount++;
	}
	else if (command == "not" || command == "neg")
		m_Ofs << "@SP\nM=M-1\nA=M\nM=" << it->second << "M\n@SP\nM=M+1\n";
}

void CodeWriter::WritePushPop(const std::string& command, const std::string& segment, int index)
{
	auto it = s_SgmtMap.find(segment);
	if (it == s_SgmtMap.end() || (command != "push" && command != "pop"))
		throw HackVM::InvalidCommand(m_CurrentFile + g_SRC_EXT);
	if (command == "push")
	{
		if (segment != "static")
			m_Ofs << "@" << index << "\nD=A\n";
		else
			m_Ofs << "@" << m_CurrentFile << "." << index << "\nD=M\n";
		if (segment == "local" || segment == "argument" || segment == "this" || segment == "that")
			m_Ofs << "@" << it->second << "\nA=M+D\nD=M\n";
		else if (segment == "pointer" || segment == "temp")
			m_Ofs << "@" << it->second << "\nA=A+D\nD=M\n";
		m_Ofs << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
	}
	else if (command == "pop")
	{
		if (segment == "constant")
			return;
		else if (segment == "static")
		{
			m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n";
			m_Ofs << "@" << m_CurrentFile << "." << index << "\nM=D\n";
			return;
		}
		if (segment == "local" || segment == "argument" || segment == "this" ||
			segment == "that") 
		{
			m_Ofs << "@" << index << "\nD=A\n@" << it->second << "\n";
			m_Ofs << "D=M+D\n@R13\nM=D\n";
		}
		else if (segment == "pointer" || segment == "temp")
		{
			m_Ofs << "@" << index << "\nD=A\n@" << it->second << "\n";
			m_Ofs << "D=A+D\n@R13\nM=D\n";
		}
		m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n@R13\nA=M\nM=D\n";	// SP-=1; M[R13]=M[SP];
	}
}