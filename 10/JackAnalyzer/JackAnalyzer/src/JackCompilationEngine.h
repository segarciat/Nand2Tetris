#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <map>
#include "JackTokenizer.h"

class JackCompilationEngine
{
public:
	// Constructs object from a sour .jack file and creates an .xml file to output.
	JackCompilationEngine(std::string sourceInName, std::string outFileName);

	// Frees input file object and output file object.
	~JackCompilationEngine();

	void CompileClass();
	void CompileClassVarDec();
	void CompileSubroutine();
	void CompileParameterList();
	void CompileVarDec();
	void CompileStatements();
	void CompileDo();
	void CompileLet();
	void CompileWhile();
	void CompileReturn();
	void CompileIf();

	void CompileExpression();
	void CompileTerm();
	void CompileExpressionList();

private:
	JackTokenizer* m_pTokenizer;
	std::ofstream* m_pOutFile;

	// Helper functions to compile terminal tokens.
	void PrintSymbol(char sym);
	void PrintKeyWord(std::string keyWordString, JackTokenizer::KeyWord keyWordToken);
	void PrintIdentifier();
	void PrintType();
};