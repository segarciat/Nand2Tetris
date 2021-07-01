#pragma once
#include <filesystem>
#include <string>
#include <fstream>
#include "JackTokenizer.h"

extern const std::string g_TARGET_EXT;

/*
* Uses the parsed Tokens from the JackTokenizer as its input
* and uses recursive descent parsing to emit an XML file in a way
* that adheres to Jack's grammar.
*/
class CompilationEngine
{
private:
	JackTokenizer m_Tokenizer;
	// Output XML file
	std::ofstream m_Ofs;

	/*
	* The following are helper methods. Each of them check for
	* more tokens and outputs the token if it is the expected one.
	*/
	void CompileSymbol(char c);
	void CompileIdentifier();
	void CompileKeyWord(const std::string& kw);

	// Outputs an identifier (user-defined type) or non-void primitive type (keyword).
	void CompileType();
	// static variables or fields.
	void CompileClassVarDec();
	void CompileSubroutine();
	void CompileParameterList();
	// Local variables, non-arguments/parameters.
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
public:
	// Creates a Tokenizer and output XML file to write to.
	CompilationEngine(const std::filesystem::path& srcPath);

	// Creates the XML for the class provided upon construction.
	void CompileClass();
};