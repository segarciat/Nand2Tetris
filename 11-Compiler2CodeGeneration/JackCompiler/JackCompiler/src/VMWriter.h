#pragma once
#include <string>
#include <fstream>

namespace jack
{
/*
* Emits VM commands to an output file.
*/
class VMWriter
{
public:
	// Virtual memory segments in VM platform.
	enum class Segment {
		CONST, ARG, LOCAL, STATIC, THIS, THAT, POINTER, TEMP
	};
	// Arithmetic commands in VM platform.
	enum class Command {
		ADD, SUB, NEG, EQ, GT, LT, AND, OR, NOT
	};
private:
	// Outfile file stream with result of generated VM commands.
	std::ofstream& m_Ofs;
	static const char* const SegmentToStr(Segment sgmt);
public:
	VMWriter(std::ofstream& ofs);
	~VMWriter();	// Release resources.
	void WritePush(Segment sgmt, int index);
	void WritePop(Segment sgmt, int index);
	void WriteArithmetic(Command cmd);
	void WriteLabel(const std::string& label);
	void WriteGoto(const std::string& label);
	void WriteIf(const std::string& label);
	void WriteCall(const std::string& name, int nArgs);
	void WriteFunction(const std::string& name, int nLocals);
	void WriteReturn();
	void Close();	// Close Output file.
};
} // namespace jack