#include "VMCodeWriter.h"
#include <fstream>
#include <sstream>

VMCodeWriter::VMCodeWriter(const std::string& fileName) :
	m_CurrentFileName(""), m_Num(0)
{
	m_pFile = new std::ofstream(fileName);
	if (!m_pFile->is_open())
		std::cout << "Unable to create file " << fileName << std::endl;
	else
		WriteInit();
}

void VMCodeWriter::WriteInit()
{
	// Bootstrap code at the top of file.
	*m_pFile << "@256\n\
D=A\n\
@SP\n\
M=D\n";

	WriteCall("Sys.init", 0);
}

void VMCodeWriter::SetFileName(const std::string& fileName)
{
	m_CurrentFileName = fileName;
}

void VMCodeWriter::WriteArithmetic(const std::string& command)
{
	std::stringstream ss;
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
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@EQZ"+std::to_string(m_Num)+"\nD;JEQ\n";
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
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@GTZ" + std::to_string(m_Num) + "\nD;JGT\n";
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
		*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@SP\nM=M-1\nA=M\nD=M-D\n@LTZ" + std::to_string(m_Num) + "\nD;JLT\n";
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

void VMCodeWriter::WriteLabel(std::string label)
{
	*m_pFile << "(" + label + ")\n";
}

void VMCodeWriter::WriteGoto(std::string label)
{
	*m_pFile << "@" << label << "\n0;JMP\n";
}

void VMCodeWriter::WriteIf(std::string label)
{
	//Get the value at the top of the stack and see if it's 0
	// @SP
	// M=M-1
	// A=M
	// D=M
	// @label
	// D;JNE
	*m_pFile << "@SP\nM=M-1\nA=M\nD=M\n@" << label << "\nD;JNE\n";
}

void VMCodeWriter::WriteFunction(std::string functionName, int numLocals)
{
	// (functionName)			// Declare function
	// @functionName$i
	// M=0						// Initialize loop control variable i to 0
	// (functionName$REPEAT)	// Local variable init loop
	// @functionName$i
	// D=M						// D=i
	// @numLocals
	// D=D-A					// D=i-numLocals
	// @functionName$END
	// D;JEQ					// goto END if D==0
	// @LCL
	// D=M						// Set D to base address stored at LCL
	// @functionName.i
	// A=D+M					// Set A to an offset of LCL by i
	// M=0						// Memory[address of A] = 0.
	// @0
	// D=A
	// @SP
	// A=M						
	// M=D						// Also push the 0 to top of stack
	// @SP
	// M=M+1					// and thus move stack pointer up by 1
	// @functionName$i
	// M=M+1					// Increment i by 1, and repeat
	// @functionName$REPEAT
	// 0;JMP
	// (functionName$END)

	// Could use the format below, but write comments on the same lines for reference.
	*m_pFile << "(" + functionName + ")\
\n@" + functionName + "$i\
\nM=0\
\n(" + functionName + "$REPEAT)\
\n@" + functionName + "$i\
\nD=M\
\n@" + std::to_string(numLocals) + "\
\nD=D-A\
\n@" + functionName + "$END\
\nD;JEQ\
\n@LCL\
\nD=M\
\n@" + functionName + ".i\
\nA=D+M\
\nM=0\
\n@0\
\nD=A\
\n@SP\
\nA=M\
\nM=D\
\n@SP\
\nM=M+1\
\n@" + functionName + "$i\
\nM=M+1\
\n@" + functionName + "$REPEAT\
\n0;JMP\
\n(" + functionName + "$END)\n";
}

void VMCodeWriter::WriteCall(std::string functionName, int numArgs)
{
	// --- push return-address ---
	// @return-address
	// D=A		// first assembly pass reads the labels... so this should work.
	// @SP
	// A=M		// A=address stored at SP
	// M=D		// Set top of stack to return address
	// @SP
	// M=M+1	// Update top stack address
	// Appending m_Num addresses the issue of label uniqueness, which may happen
	// when functions are called recursively.
	m_Num++;
	*m_pFile << "@" + functionName + "$return-address" + std::to_string(m_Num) + "\
\nD=A\
\n@SP\
\nA=M\
\nM=D\
\n@SP\
\nM=M+1\n";

	// --- push SEGMENT ---
	// @SEGMENT
	// D=M		// D is value stored at A, i.e., D=*(SEGMENT)
	// @SP
	// A=M		// A=address stored at SP
	// M=D		// Set top of stack to SEGMENT
	// @SP
	// M=M+1	// Update top stack address
	const std::string segments[] = { "LCL", "ARG", "THIS", "THAT" };
	for (std::string segment : segments)
	{
		*m_pFile << "@" + segment + "\
\nD=M\
\n@SP\
\nA=M\
\nM=D\
\n@SP\
\nM=M+1\n";
	}

	// --- ARG = SP-n-5 ---
	// @SP
	// D=M			// D=value at SP
	// @numArgs
	// D=D-A
	// @5
	// D=D-A		// D= SP-n-5
	// @ARG
	// M=D			// ARG = SP-n-5
	*m_pFile << "@SP\
\nD=M\
\n@" + std::to_string(numArgs) + "\
\nD=D-A\
\n@5\
\nD=D-A\
\n@ARG\
\nM=D\n";


// --- LCL = SP ---
// @SP
// D=M
// @LCL
// M=D			// LCL = SP
	*m_pFile << "@SP\
\nD=M\
\n@LCL\
\nM=D\n";

	// --- goto functionName ---
	// @functionName
	// 0;JMP

	*m_pFile << "@" + functionName + "\n0;JMP\n";

	// (return-address)
	*m_pFile << "(" + functionName + "$return-address" +std::to_string(m_Num)+")\n";
}

void VMCodeWriter::WriteReturn()
{
	// FRAME=LCL temp variable
	// --- FRAME=LCL ---
	// @LCL
	// D=M				// D=LCL
	// @FRAME			
	// M=D				// FRAME = LCL
	*m_pFile << "@LCL\n\
D=M\n\
@FRAME\n\
M=D\n";

	// --- RET=*(FRAME-5) ---
	// @5
	// A=D-A			// A=FRAME-5
	// D=M				// D=*(FRAME-5)... value at FRAME-5
	// @RET
	// M=D				// RET=D
	*m_pFile << "@5\n\
A=D-A\n\
D=M\n\
@RET\n\
M=D\n";

	// --- *ARG = pop() ---
	// @SP
	// M=M-1			// Move SP down
	// A=M				// A points to top of stack
	// D=M				// D=value at top of stack
	// @ARG
	// A=M				// A points to base ARG address
	// M=D
	*m_pFile << "@SP\n\
M=M-1\n\
A=M\n\
D=M\n\
@ARG\n\
A=M\n\
M=D\n";

	// --- * SP = ARG + 1
	// @ARG
	// D=M
	// @1
	// D=D+A			// D=ARG+1
	// @SP
	// M=D				// SP=ARG+1
	*m_pFile << "@ARG\n\
D=M\n\
@1\n\
D=D+A\n\
@SP\n\
M=D\n";

	// --- THAT = *(FRAME-i)
	// @FRAME
	// D=M				// D=value at frame
	// @i
	// A=D-A
	// D=M
	// @SEGMENT
	// M=D
	const std::string segments[] = { "THAT", "THIS", "ARG", "LCL" };
	for (int i = 1; i < 5; i++ )
	{
		*m_pFile << "@FRAME\n\
D=M\n\
@" + std::to_string(i) + "\n\
A=D-A\n\
D=M\n\
@" + segments[i-1] + "\n\
M=D\n";
	}

	// --- goto RET ---
	// @RET
	// A=M			\\ set A to address stored at RET
	// 0;JMP
	* m_pFile << "@RET\n\
A=M\n\
0;JMP\n";
}

void VMCodeWriter::Close()
{
	m_pFile->close();
}