#pragma once

#include <iostream>
#include <fstream>

/**
	Purpose: Emits VM commands into a file, using the VM command syntax.
*/

class VMWriter
{
public:
	enum class Segment
	{
		CONST = 0, ARG, LOCAL, STATIC,
		THIS, THAT, POINTER, TEMP, NONE
	};

	enum class Command
	{
		ADD, SUB, NEG, EQ, GT,
		LT, AND, OR, NOT
	};

	// Creates a new file and prepares it for writing.
	VMWriter(std::string& fileName);
	~VMWriter();

	// Functions that write VM commands.
	void WritePush(Segment segment, int index);
	void WritePop(Segment segment, int index);
	void WriteArithmetic(Command command);
	void WriteLabel(const std::string& label);
	void WriteGoto(const std::string& label);
	void WriteIf(const std::string& label);
	void WriteCall(const std::string& name, int nArgs);
	void WriteFunction(const std::string&, int nLocals);
	void WriteReturn();

	// Closes the output file.
	void Close();

private:
	std::ofstream* m_pOutFile;
};