#pragma once
#include <string>
#include <fstream>
#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

namespace jack {
/*
* Uses the parsed Tokens from the JackTokenizer as its input
* and uses recursive descent parsing to emit an XML file in a way
* that adheres to Jack's grammar.
*/
class CompilationEngine
{
public:
	// Creates a Tokenizer and output XML file to write to.
	CompilationEngine(std::ifstream& ifs, std::ofstream& ofs, const std::string& className);

	// Creates the XML for the class provided upon construction.
	void CompileClass();
private:
	JackTokenizer m_Tokenizer;
	// Contains information about each variable in the current scope.
	SymbolTable m_ST;
	// Emits VM commands based on tokenized input.
	VMWriter m_Writer;
	// Name of class being written.
	std::string m_ClassName;
	// Used for label uniqueness.
	int m_LabelCount;

	// Static variables or fields.
	void CompileClassVarDec();
	// Function, method, or constructor declaration (definition), not call.
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
	// Returns number of arguments in list for use in subroutine call.
	int CompileExpressionList();

	/*
	* The following are helper methods. Each of them check for
	* more tokens and outputs the token if it is the expected one.
	*/
	void CompileSymbol(char c);
	void CompileIdentifier();
	void CompileKeyWord(const std::string& kw);
	// Outputs user-defined type or non-void primitive type; returns type.
	const std::string CompileType();

	// Compiles and computes array entry.
	void ComputeArrayOffset(VMWriter::Segment sgmt, int index);
	// Compiles a subroutine call with where return value (if any) is ignored.
	void CompileSubroutineCall(std::string& name);
	// Maps a variable name to its corresponding segment.
	VMWriter::Segment NameToSgmt(const std::string& name);
};
} // namespace jack
