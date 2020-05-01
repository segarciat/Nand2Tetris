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