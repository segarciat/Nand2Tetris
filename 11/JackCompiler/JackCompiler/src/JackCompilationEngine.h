#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <map>

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

/**
	Purpose: Effects the compilation output from a .jack -> .vm
	translation. Gets its input from a JackTokenizer, and emits its
	parsed structure into an output file.
*/

class JackCompilationEngine
{
public:
	// Allocates memory for a JackTokenizer object and a VMWriter object.
	JackCompilationEngine(std::string& sourceInName, std::string& outName);

	// Frees JackTokenizer and VMWriter memory.
	~JackCompilationEngine();

	// Compiles a complete class.
	void CompileClass();

	// Compiles static/field declarations.
	void CompileClassVarDec();

	// Compiles a complete method, function, or constructor.
	void CompileSubroutine();

	// Compiles a (possibly empty) parameter list, not including delimiting "( )".
	void CompileParameterList();

	// Compiles a var (local) declaration.
	void CompileVarDec();

	// Compiles a sequence of statements, not including the delimiting "{ }".
	void CompileStatements();

	// Compiles a do statement.
	void CompileDo();

	// Compiles a let statement.
	void CompileLet();
	
	// Compiles a while statement.
	void CompileWhile();

	// Compiles a return statement.
	void CompileReturn();

	// Compiles an if statement, with a possibly trailing else.
	void CompileIf();

	// Compiles an expression.
	void CompileExpression();

	// Compiles a term.
	void CompileTerm();

	// Compiles a (possibly empty) comma-separated list of expressions.
	void CompileExpressionList();

private:
	// Token object that tokenizes the source file input.
	JackTokenizer* m_pTokenizer;

	// VMWriter that writes output VM code.
	VMWriter* m_pCodeWriter;

	// Symbol table that keeps track of variable identifiers.
	SymbolTable m_SymbolTable;

	// The current identifier struct.
	SymbolTable::Identifier m_CurrentIdentifier;

	// The name of the class being compiled by the current JackCompilationEngine.
	std::string m_ClassName;

	// Counter appended to labels for uniqueness.
	int m_LabelCounter;

	// Advances upon encountering a symbol.
	void PrintSymbol();

	// Advances upon encountering a keyword.
	void PrintKeyWord(std::string keyWordString, JackTokenizer::KeyWord keyWordToken);

	// Advances upon encountering an identifier, and sets the current identifier name.
	void PrintIdentifier();

	// Advances upon encountering a variable type, and sets the current idetnfier type.
	void PrintType();

	// Gets the segment corresponding to a variable name.
	VMWriter::Segment GetVarSegment(std::string& varName);
};