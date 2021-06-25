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
* Bootstrap code for initializaiton. Positions stack pointer SP at 256,
* and then hands control to Sys.init, which, among other initialization
* tasks, calls Main.main.
*/
void CodeWriter::WriteInit()
{
	m_Ofs << "@256\nD=A\n@SP\nM=D\n";
	m_CurrentFunction = "Sys.init";
	WriteCall(m_CurrentFunction, 0);
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