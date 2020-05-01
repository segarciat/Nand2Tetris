#pragma once

#include <iostream>
#include <fstream>

#include "HackParser.h"
#include "HackCode.h"
#include "SymbolTable.h"


/* 
	Purpose: The HackWriter reads a file with assembly language mnemonics, written 
	for the Hack system, into machine (binary) code for that same system. 
	The expected input filename should not contain a file extension, since 
	the writer will use the name to both read the assembly file and create 
	a hack file.
*/

class HackWriter
{
public:
	HackWriter(std::string& sourceName);
	~HackWriter();

	void WriteHackCode();

private:
	HackParser* m_pParser;

	// Converts assembly language mnemonics to binary codes.
	HackCode m_CodeGenerator;

	// Maps symbolic labels to numeric addresses.
	SymbolTable m_SymbolTable;

	int m_ROMLines;
	int m_RAMAddress;
	std::ofstream* m_pOutHackFile;

	void BuildSymbols();
	std::string To16BitBinary(int decimal);
};