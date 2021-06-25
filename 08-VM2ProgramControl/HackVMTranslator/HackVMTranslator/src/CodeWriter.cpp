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

void CodeWriter::WriteInit()
{
	m_Ofs << "@256\nD=A\n@SP\nM=D\n";
	m_CurrentFunction = "Sys.init";
	WriteCall(m_CurrentFunction, 0);
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

const std::string CodeWriter::UniqueLabel(const std::string& label)
{	// (functionName$label:labelCount)
	return m_CurrentFunction + "$" + label;
}

/* 
* Writes a unique label that is used by jump (goto) commands.
*/
void CodeWriter::WriteLabel(const std::string& label)
{
	m_Ofs << "(" << UniqueLabel(label) << ")\n";
}

/*
* Writes commands that effect an unconditional jump to a label.
*/
void CodeWriter::WriteGoto(const std::string& label)

{
	m_Ofs << "@" << UniqueLabel(label) << "\n0;JMP\n";
}

/*
* Writes commands that effect a conditional jump to a label.
*/
void CodeWriter::WriteIf(const std::string& label)
{
	m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n";						// Pop value from stack
	m_Ofs << "@" << UniqueLabel(label) << "\nD;JNE\n";		// Jump if it's nonzero
}

/*
* Writes commands that effect a function call.
*/
void CodeWriter::WriteCall(const std::string& functionName, int numArgs)
{
	// Temporarily save (push) return address (prepend integer for label uniqueness)
	m_Ofs << "@_" << ++m_LabelCount << UniqueLabel("RETURN") << "\n";
	m_Ofs << "D=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
	
	// Push current function's stack frame
	for (const std::string& sgmt : { "@LCL", "@ARG", "@THIS", "@THAT" })
		m_Ofs << sgmt << "\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";

	// Reposition ARG pointer for invoked function
	m_Ofs << "@SP\nD=M\n@" << numArgs << "\nD=D-A\n@5\nD=D-A\n";	// D = SP-n-5
	m_Ofs << "@ARG\nM=D\n";											// ARG = D

	// Reposition LCL pointer to top of stack: LCL=SP
	m_Ofs << "@SP\nD=M\n@LCL\nM=D\n";

	// Jump to called function address
	m_Ofs << "@" << functionName << "\n0;JMP\n";

	// Label associated with current function's return address
	m_Ofs << "(_" << m_LabelCount << UniqueLabel("RETURN") << ")\n";
}

/* 
* Writes commands to effect returning from a called function to the caller.
*/
void CodeWriter::WriteReturn()
{
	// Temporarily store top of caller's stack frame: FRAME=LCL
	m_Ofs << "@LCL\nD=M\n@R13\nM=D\n";

	// Temporarily save return address to jump back to caller: RET = *(FRAME-5)
	m_Ofs << "@R13\nD=M\n@5\nA=D-A\nD=M\n@R14\nM=D\n";

	// Place return value at top of stack for caller: *ARG = pop()
	m_Ofs << "@SP\nM=M-1\nA=M\nD=M\n@ARG\nA=M\nM=D\n";

	// Reposition stack pointer above return value
	m_Ofs << "@ARG\nD=M+1\n@SP\nM=D\n";

	// Restore the stack from of caller: SGMT = *(FRAME-offset)
	int offset = 1;
	for (const std::string& sgmt: {"@THAT", "@THIS", "@ARG", "@LCL"})
		m_Ofs << "@" << offset++ << "\nD=A\n@R13\nA=M-D\nD=M\n" << sgmt << "\nM=D\n";

	// Resume execution at the caller
	m_Ofs << "@R14\nA=M\n0;JMP\n";
}

/* 
* Writes commands that create a label for a function and intializes locals to 0.
*/
void CodeWriter::WriteFunction(const std::string& functionName, int numLocals)
{
	m_CurrentFunction = functionName;
	m_Ofs << "(" << m_CurrentFunction << ")\n";
	while (numLocals--)
		m_Ofs << "@SP\nA=M\nM=0\n@SP\nM=M+1\n";
}