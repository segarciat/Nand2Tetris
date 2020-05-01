#include "VMCodeWriter.h"
#include <fstream>
#include <sstream>

VMCodeWriter::VMCodeWriter(const std::string& fileName) :
	m_CurrentFileName(""), m_Num(0)
{
	m_pFile = new std::ofstream(fileName);
	if (!m_pFile->is_open())
		std::cout << "Unable to create file " << fileName << std::endl;
}

void VMCodeWriter::SetFileName(const std::string& fileName)
{
	m_CurrentFileName = fileName;
}

void VMCodeWriter::WriteArithmetic(const std::string& command)
{
	if (command.find("add") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=D+M\n@SP\nM=M+1\n";
		// Get the two values off the top of the stack.
		// @SP	Set A to Memory[0], for example, 258
		// M=M-1  // Decrement it by 1: Memory[0] = 258 - 1 (make it 257)
		// A=M		// Go to address 257 (A=Memory[0])
		// D=M		// D=RAM[257]
		// @SP		// Set A to 0
		// M=M-1	// Set Memory[0] = 256
		// A=M		// Set A to 256 (A=Memory[0])
		// M=D+M	// Memory[256]=Memory[257]+Memory[256]
		// @SP
		// M=M+1	// SP=SP+1 -> SP=257
	}
	else if (command.find("sub") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=M-D\n@SP\nM=M+1\n";
		// @SP	Set A to Memory[0], for example, 258
		// M=M-1  // Decrement it by 1: Memory[0] = 258 - 1 (make it 257)
		// A=M		// Go to address 257 (A=Memory[0])
		// D=M		// D=RAM[257]
		// @SP		// Set A to 0
		// M=M-1	// Set Memory[0] = 256
		// A=M		// Set A to 256 (A=Memory[0])
		// M=M-D	// Memory[256]=Memory[256]-Memory[257]
		// @SP
		// M=M+1	// SP=SP+1 -> SP=257
	}
	else if (command.find("neg") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nM=-M\n@SP\nM=M+1\n";
		// @SP	Set A to Memory[0], for example, 257
		// M=M-1  // Decrement it by 1: Memory[0] = 257 - 1 (make it 256)
		// A=M		// Go to address 256 (A=Memory[0])
		// M=-M		// Memory[256]=!Memory[256]
		// @SP
		// M=M+1	// Memory[0]=257
	}
	else if (command.find("eq") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@EQZ"+std::to_string(m_Num)+"\n\D;JEQ\n";
		*m_pFile << "D=0\n@END" + std::to_string(m_Num) + "\n0;JMP\n(EQZ" + std::to_string(m_Num) + ")\nD=-1\n(END" + std::to_string(m_Num) + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		// @SP	Set A to Memory[0], for example, 258
		// M=M-1  // Decrement it by 1: Memory[0] = 258 - 1 (make it 257)
		// A=M		// Go to address 257 (A=Memory[0])
		// D=M		// D=RAM[257]
		// @SP		// Set A to 0
		// M=M-1	// Set Memory[0] = 256
		// A=M		// Set A to 256 (A=Memory[0])
		// D=M-D	// D=Memory[256]-Memory[257]
		// @EQZ
		// D;JEQ
		// D=0
		// @END
		// 0;JMP
		// (EQZ)
		// D=-1
		// (END)
		// @SP
		// A=M // A=Memory[0]=256
		// M=D	// Memory[256] = 0 (or -1)
		// @SP	// A=0
		//M=M+1 // Memory[0] = Memory[0]+1 -> move stack pointer up
		m_Num++;
	}
	else if (command.find("gt") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@GTZ" + std::to_string(m_Num) + "\n\D;JGT\n";
		*m_pFile << "D=0\n@END" + std::to_string(m_Num) + "\n0;JMP\n(GTZ" + std::to_string(m_Num) + ")\nD=-1\n(END" + std::to_string(m_Num) + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		// @SP	Set A to Memory[0], for example, 258
		// M=M-1  // Decrement it by 1: Memory[0] = 258 - 1 (make it 257)
		// A=M		// Go to address 257 (A=Memory[0])
		// D=M		// D=RAM[257]
		// @SP		// Set A to 0
		// M=M-1	// Set Memory[0] = 256
		// A=M		// Set A to 256 (A=Memory[0])
		// D=M-D	// D=Memory[256]-Memory[257]
		// @EQ
		// D;JGT
		// D=0
		// @END
		// 0;JMP
		// (JGT)
		// D=-1
		// (END)
		// @SP
		// A=M // A=Memory[0]=256
		// M=D	// Memory[256] = 0 (or -1)
		//@SP	// A=0
		//M=M+1 // Memory[0] = Memory[0]+1 -> move stack pointer up
		m_Num++;
	}
	else if(command.find("lt") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@LTZ" + std::to_string(m_Num) + "\n\D;JLT\n";
		*m_pFile << "D=0\n@END" + std::to_string(m_Num) + "\n0;JMP\n(LTZ" + std::to_string(m_Num) + ")\nD=-1\n(END" + std::to_string(m_Num) + ")\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		// @SP	Set A to Memory[0], for example, 258
		// M=M-1  // Decrement it by 1: Memory[0] = 258 - 1 (make it 257)
		// A=M		// Go to address 257 (A=Memory[0])
		// D=M		// D=RAM[257]
		// @SP		// Set A to 0
		// M=M-1	// Set Memory[0] = 256
		// A=M		// Set A to 256 (A=Memory[0])
		// D=M-D	// D=Memory[256]-Memory[257]
		// @LTZ
		// D;JLT
		// D=0
		// @END
		// 0;JMP
		// (LTZ)
		// D=-1
		// (END)
		// @SP
		// A=M // A=Memory[0]=256
		// M=D	// Memory[256] = 0 (or -1)
		//@SP	// A=0
		//M=M+1 // Memory[0] = Memory[0]+1 -> move stack pointer up
		m_Num++;
	}
	else if (command.find("and") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=D&M\n@SP\nM=M+1\n";
		// Get the two values off the top of the stack.
		// @SP	Set A to Memory[0], for example, 258
		// M=M-1  // Decrement it by 1: Memory[0] = 258 - 1 (make it 257)
		// A=M		// Go to address 257 (A=Memory[0])
		// D=M		// D=RAM[257]
		// @SP		// Set A to 0
		// M=M-1	// Set Memory[0] = 256
		// A=M		// Set A to 256 (A=Memory[0])
		// M=D&M	// Memory[256]=Memory[257] AND Memory[256]
		// @SP
		// M=M+1	// SP=SP+1 -> SP=257
	}
	else if (command.find("or") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nM=D|M\n@SP\nM=M+1\n";
		// Get the two values off the top of the stack.
		// @SP	Set A to Memory[0], for example, 258
		// M=M-1  // Decrement it by 1: Memory[0] = 258 - 1 (make it 257)
		// A=M		// Go to address 257 (A=Memory[0])
		// D=M		// D=RAM[257]
		// @SP		// Set A to 0
		// M=M-1	// Set Memory[0] = 256
		// A=M		// Set A to 256 (A=Memory[0])
		// M=D|M	// Memory[256]=Memory[257] OR Memory[256]
		// @SP
		// M=M+1	// SP=SP+1 -> SP=257
	}
	else if (command.find("not") != -1)
	{
		*m_pFile << "@SP\nM=M-1\nA=M\nM=!M\n@SP\nM=M+1\n";
		// @SP	Set A to Memory[0], for example, 257
		// M=M-1  // Decrement it by 1: Memory[0] = 257 - 1 (make it 256)
		// A=M		// Go to address 256 (A=Memory[0])
		// M=!M		// Memory[256]=!Memory[256]
		// @SP
		// M=M+1	// Memory[0]=257
	}
}

void VMCodeWriter::WritePushPop(VMParser::CommandType command, const std::string& segment, int index)
{
	if (command == VMParser::CommandType::C_PUSH)
	{
		// Check what segment
		if (segment.find("constant") != -1)
		{
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
			return;
			// @index
			// D=A
			// @SP // 
			// A=M // A=Memory[0], i.e., 256
			// M=D
			// @SP
			// M=M+1
		}
		if (segment.find("static") != -1)
		{
			*m_pFile << "@" + m_CurrentFileName + "." + std::to_string(index) + "\nD=M\n";
			*m_pFile << "@SP\nA=M\nM=D\n@SP\nM=M+1\n";
			return;
		}
		// @index
		// D=A		// D= index
		// @LCL
		// A=D+M	// A= index + Memory[1]
		// D=M		// D= Memory[Memory[1] + index]

		// @SP
		// A=M		// A = Memory[0]
		// M=D		// Memory[Memory[0]] = ...
		// @SP
		// M=M+1	// Move stack pointer up
		if (segment.find("local") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@LCL\nA=D+M\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		else if (segment.find("argument") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@ARG\nA=D+M\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		else if (segment.find("this") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@THIS\nA=D+M\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		else if (segment.find("that") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@THAT\nA=D+M\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		// For pointer or temp...
		// @index
		// D=A		// D= index
		// @5
		// A=D+A	// A= index + 5
		// D=M

		// @SP
		// A=M		// A = Memory[0]
		// M=D		// Memory[Memory[0]] = ...
		// @SP
		// M=M+1	// Move stack pointer up
		else if (segment.find("pointer") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@3\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		else if (segment.find("temp") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@5\nA=D+A\nD=M\n@SP\nA=M\nM=D\n@SP\nM=M+1\n";
		// For static...
		// @filename.index
		// D=M

		// @SP
		// A=M		// A = Memory[0]
		// M=D		// Memory[Memory[0]] = ...
		// @SP
		// M=M+1	// Move stack pointer up
	}
	else if (command == VMParser::CommandType::C_POP)
	{
		if (segment.find("constant") != -1)
		{
			// This means we just return the constant passed in
			return;
		}
		// For static variable..
		// Get top stack value
		// @SP
		// M=M-1
		// A=M		// A=Memory[0]
		// D=M		// Topmost stack value

		// Place it at @address...
		// @filename.index
		// M=D
		if (segment.find("static") != -1)
		{
			*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@" + m_CurrentFileName + "." + std::to_string(index) + "\nM=D\n";
			return;
		}

		// @index
		// D=A		//D = index
		// @LCL
		// D=D+A
		// @address
		// M=D		// Memory[address]= Memory[1] + index

		// Get top stack value
		// @SP
		// M=M-1
		// A=M		// A=Memory[0]
		// D=M		// Topmost stack value

		// Place it at @address...
		// @address
		// A=M		// A=memory address of interest... so A=Memory[1] + index
		// M=D		// Memory[Memory[1] + index] = topmost stack value
		if (segment.find("local") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@LCL\nD=D+M\n@address" + std::to_string(m_Num) + "\n";
		else if (segment.find("argument") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@ARG\nD=D+M\n@address" + std::to_string(m_Num) + "\n";
		else if (segment.find("this") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@THIS\nD=D+M\n@address" + std::to_string(m_Num) + "\n";
		else if (segment.find("that") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@THAT\nD=D+M\n@address" + std::to_string(m_Num) + "\n";
		//For pointer or temp...
		// @index
		// D=A		//D = index
		// @5
		// D=D+A
		// @address
		// M=D		// Memory[address]= Memory[1] + index

		// Get top stack value
		// @SP
		// M=M-1
		// A=M		// A=Memory[0]
		// D=M		// Topmost stack value

		// Place it at @address...
		// @address
		// A=M		// A=memory address of interest... so A=Memory[1] + index
		// M=D		// Memory[Memory[1] + index] = topmost stack value
		else if (segment.find("pointer") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@3\nD=D+A\n@address" + std::to_string(m_Num) + "\n";
		else if (segment.find("temp") != -1)
			*m_pFile << "@" + std::to_string(index) + "\nD=A\n@5\nD=D+A\n@address" + std::to_string(m_Num) + "\n";
		*m_pFile << "M=D\n@SP\nM=M-1\nA=M\nD=M\n@address" + std::to_string(m_Num) + "\nA=M\nM=D\n";
	}
	m_Num++;
}

void VMCodeWriter::Close()
{
	m_pFile->close();
}