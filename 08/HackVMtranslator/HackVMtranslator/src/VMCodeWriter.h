#pragma once

#include <iostream>
#include <fstream>
#include "VMParser.h"

/**
	Purpose: Translates VM commands into Hack assembly code.
*/

class VMCodeWriter
{
public:
	VMCodeWriter(const std::string& fileName);

	// Informs the code writer that the translation of a new VM file has started.
	void SetFileName(const std::string& fileName);

	// Writes the assembly code translation of an arithmetic command.
	void WriteArithmetic(const std::string& command);

	// Writes the assembly code that is the translation of the given push or pop command.
	void WritePushPop(VMParser::CommandType, const std::string& segment, int index);

	// Writes assembly code that effects the VM initialization, aka bootstrap code.
	void WriteInit();

	// Writes assembly code that effects the label command.
	void WriteLabel(std::string label);

	// Write assembly code that effects the goto command.
	void WriteGoto(std::string label);

	// Write assembly code that effects hte if-goto command.
	void WriteIf(std::string label);

	// Writes assembly code that effecst the call command.
	void WriteCall(std::string functionName, int numArgs);

	// Writes assembly code taht effecst the return command.
	void WriteReturn();

	// Writes assembly code.
	void WriteFunction(std::string functionName, int numLocals);

	// Closes the output file.
	void Close();
	
private:
	// The output assembly file.
	std::ofstream* m_pFile;

	// The name of the VM file being translated.
	std::string m_CurrentFileName;

	//Keeps track of line number on output file.
	//Used for @label and (label) command pairs.
	int m_Num;
};