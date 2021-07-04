#include "CompilationEngine.h"
#include "JackConstants.h"		// jack namespace string literal constants
#include "JackTokenError.h"
#include "JackIdentifierError.h"

namespace jack {

CompilationEngine::CompilationEngine(std::ifstream& ifs, std::ofstream& ofs, const std::string& className)
	:m_Tokenizer{ ifs }, m_Writer{ ofs }, m_ClassName{ className }, m_LabelCount{ 0 } 
{}

/* Compiles a class with expected Jack syntax:
* class ClassName {
* (field and static declarations)*
* (subroutine declarations)*
* }
*/
void CompilationEngine::CompileClass()
{
	if (m_Tokenizer.HasMoreTokens())
	{
		// Read in next token and set current.
		m_Tokenizer.Advance();
		if (m_Tokenizer.TokenType() != JackTokenType::KEYWORD)
			throw JackTokenError("Expected class");
		CompileKeyWord(jack::CLASS);
		CompileIdentifier();
		CompileSymbol('{');
		// All (if any) class variables must be declared at start of class.
		std::string kw = m_Tokenizer.KeyWord();
		JackTokenType t = m_Tokenizer.TokenType();
		while (t == JackTokenType::KEYWORD && (kw == jack::STATIC || kw == jack::FIELD))
		{
			CompileClassVarDec();
			t = m_Tokenizer.TokenType();
			kw = m_Tokenizer.KeyWord();
		}
		// All (if any) subroutine declarations follow class variable declarations.
		while (t == JackTokenType::KEYWORD &&(kw == jack::METHOD || 
					kw == jack::CONSTRUCTOR || kw == jack::FUNCTION))
		{
			CompileSubroutine();
			t = m_Tokenizer.TokenType();
			kw = m_Tokenizer.KeyWord();
		}
		CompileSymbol('}');
	}
}

/* Compiles Jack variable declarations with expected syntax:
* field type identifier_1, ..., identifier_n;
* or .. say
* static type identifier_1,..., identifier_n;
*/
void CompilationEngine::CompileClassVarDec()
{
	const std::string scope_kw = m_Tokenizer.KeyWord();
	CompileKeyWord(scope_kw);
	SymbolTable::Kind kind = (scope_kw == jack::FIELD) ? 
			SymbolTable::Kind::FIELD : SymbolTable::Kind::STATIC;
	const std::string type = CompileType();
	m_ST.Define(m_Tokenizer.Identifier(), type, kind);	// Add variable to Symbol Table.
	CompileIdentifier();
	// Possibly declaring multiple variables at once.
	while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL &&
		m_Tokenizer.Symbol() == ',')
	{
		CompileSymbol(',');
		m_ST.Define(m_Tokenizer.Identifier(), type, kind);
		CompileIdentifier();
	}
	CompileSymbol(';');
}
/* Compiles a jack constructor, function (static method), or instance method:
* subroutineKeyword subroutineName(parameterList)
* {
*	// (variable declarations)*
*	// *(statements)*
* }
*/
void CompilationEngine::CompileSubroutine()
{
	// Start subroutine header.
	m_ST.StartSubroutine();
	const std::string& kw = m_Tokenizer.KeyWord();	// "function", "method", "constructor".
	CompileKeyWord(kw);
	// Return type may be void, primitive-type, or user-defined type.
	if (m_Tokenizer.TokenType() == JackTokenType::KEYWORD && 
			m_Tokenizer.KeyWord() == jack::VOID)
		CompileKeyWord(jack::VOID);
	else
		CompileType();
	// Used below in writing a VM 'function' command.
	const std::string& funcName = m_ClassName + "." + m_Tokenizer.Identifier();
	CompileIdentifier();
	if (kw == jack::METHOD)			// First argument to instance method is 'this'.
		m_ST.Define(jack::THIS, m_ClassName, SymbolTable::Kind::ARG);
	CompileSymbol('(');
	CompileParameterList();
	CompileSymbol(')');
	// End header and begin Subroutine body.
	CompileSymbol('{');
	// All variable declarations must appear at start of subroutine body.
	while (m_Tokenizer.TokenType() == JackTokenType::KEYWORD &&
			m_Tokenizer.KeyWord() == jack::VAR)
		CompileVarDec();
	int nLocals = m_ST.VarCount(SymbolTable::Kind::VAR);
	m_Writer.WriteFunction(funcName, nLocals);
	if (kw == jack::METHOD)
	{	// First argument is "this"; set its base address in current scope.		
		m_Writer.WritePush(VMWriter::Segment::ARG, 0);
		m_Writer.WritePop(VMWriter::Segment::POINTER, 0);
	} 
	else if (kw == jack::CONSTRUCTOR)
	{	// Allocate memory for object.
		m_Writer.WritePush(VMWriter::Segment::CONST, m_ST.VarCount(SymbolTable::Kind::FIELD));
		m_Writer.WriteCall("Memory.alloc", 1);	// Returns base address of object created.
		m_Writer.WritePop(VMWriter::Segment::POINTER, 0);	// Set "this" to that base address.
	}
	CompileStatements();
	CompileSymbol('}');
}

void CompilationEngine::CompileParameterList()
{
	// Possibly empty parameter list.
	if (m_Tokenizer.TokenType() != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != ')')
	{
		std::string type = CompileType();
		m_ST.Define(m_Tokenizer.Identifier(), type, SymbolTable::Kind::ARG);
		CompileIdentifier();
		// If non-empty, could have more than one parameter.
		while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == ',')
		{
			CompileSymbol(',');
			CompileType();
			m_ST.Define(m_Tokenizer.Identifier(), type, SymbolTable::Kind::ARG);
			CompileIdentifier();
		}
	}
}

void CompilationEngine::CompileVarDec()
{
	CompileKeyWord(jack::VAR);
	std::string type = CompileType();
	m_ST.Define(m_Tokenizer.Identifier(), type, SymbolTable::Kind::VAR);
	CompileIdentifier();
	// Possibly more than one variable in same declaration.
	while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == ',')
	{
		CompileSymbol(',');
		m_ST.Define(m_Tokenizer.Identifier(), type, SymbolTable::Kind::VAR);
		CompileIdentifier();
	}
	CompileSymbol(';');
}

void CompilationEngine::CompileStatements()
{
	while (m_Tokenizer.TokenType() == JackTokenType::KEYWORD)
	{
		const std::string& kw = m_Tokenizer.KeyWord();
		if (kw == jack::DO)
			CompileDo();
		else if (kw == jack::LET)
			CompileLet();
		else if (kw == jack::WHILE)
			CompileWhile();
		else if (kw == jack::RETURN)
			CompileReturn();
		else if (kw == jack::IF)
			CompileIf();
		else
			throw JackTokenError(std::string("Unexpected keyword ") + kw);
	}
}


// do subroutineName();
void CompilationEngine::CompileDo()
{
	CompileKeyWord(jack::DO);
	// Ambiguous: class name, subroutine name, or variable name.
	std::string name = m_Tokenizer.Identifier();
	if (std::isupper(name[0]))			// E.g. do Output.print("Hello"); 
		CompileIdentifier();
	else
		CompileIdentifier();
	CompileSubroutineCall(name);
	CompileSymbol(';');
	m_Writer.WritePop(VMWriter::Segment::TEMP, 0);	// Ignore return value.
}

/* Jack assignment statement:
* Example 1: let x = 5 * 8;
* Example 2: let a[i] = 2 * a[i-1];
*/
void CompilationEngine::CompileLet()
{
	CompileKeyWord(jack::LET);
	const std::string name = m_Tokenizer.Identifier();
	CompileIdentifier();
	// Name and segment may change in array entry.
	VMWriter::Segment sgmt = NameToSgmt(name);
	int index = m_ST.IndexOf(name);
	if (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == '[')
	{	// Assigning to array entry.
		ComputeArrayOffset(sgmt, index);
		sgmt = VMWriter::Segment::THAT;
		index = 0;
	}
	CompileSymbol('=');
	// Expression may be an array entry, like a[i-1]; may affect 'that' base above.
	CompileExpression();
	CompileSymbol(';');
	// Complete assignment, e.g., a[5] = 10; or a[i] = 2 * a[i-1]);
	m_Writer.WritePop(sgmt, index);
}

void CompilationEngine::CompileWhile()
{
	const std::string end_while_label = std::string("END_WHILE") + std::to_string(m_LabelCount);
	const std::string while_test_label = std::string("WHILE_RETRY") + std::to_string(m_LabelCount);
	m_LabelCount++;
	CompileKeyWord(jack::WHILE);
	m_Writer.WriteLabel(while_test_label);
	CompileSymbol('(');
	CompileExpression();
	CompileSymbol(')');
	m_Writer.WriteArithmetic(VMWriter::Command::NOT);
	m_Writer.WriteIf(end_while_label);
	CompileSymbol('{');
	CompileStatements();
	m_Writer.WriteGoto(while_test_label);
	CompileSymbol('}');
	m_Writer.WriteLabel(end_while_label);
}

void CompilationEngine::CompileReturn()
{
	CompileKeyWord(jack::RETURN);
	
	// Compute return value.
	if (m_Tokenizer.TokenType() != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != ';')
		CompileExpression();
	else // Empty return statement; push 0.
		m_Writer.WritePush(VMWriter::Segment::CONST, 0);
	CompileSymbol(';');
	m_Writer.WriteReturn();
}

void CompilationEngine::CompileIf()
{
	const std::string else_label = std::string("ELSE") + std::to_string(m_LabelCount);
	const std::string end_if_label = std::string("END_IF") + std::to_string(m_LabelCount);
	m_LabelCount++;
	CompileKeyWord(jack::IF);
	CompileSymbol('(');
	CompileExpression();
	CompileSymbol(')');
	m_Writer.WriteArithmetic(VMWriter::Command::NOT);
	m_Writer.WriteIf(else_label);
	CompileSymbol('{');
	CompileStatements();
	CompileSymbol('}');
	m_Writer.WriteGoto(end_if_label);
	m_Writer.WriteLabel(else_label);
	// Optional else statement following if.
	if (m_Tokenizer.TokenType() == JackTokenType::KEYWORD && 
			m_Tokenizer.KeyWord() == jack::ELSE)
	{
		CompileKeyWord(jack::ELSE);
		CompileSymbol('{');
		CompileStatements();
		CompileSymbol('}');
	}
	m_Writer.WriteLabel(end_if_label);
}

void CompilationEngine::CompileExpression()
{
	CompileTerm();
	if (m_Tokenizer.TokenType() == JackTokenType::SYMBOL)
	{
		char op = m_Tokenizer.Symbol();
		if (op == '+' || op == '-' || op == '*' || op == '/' ||
			op == '&' || op == '|' || op == '<' || op == '>' ||
			op == '=')
		{
			CompileSymbol(op);
			CompileTerm();
			if (op == '+')
				m_Writer.WriteArithmetic(VMWriter::Command::ADD);
			else if (op == '-')
				m_Writer.WriteArithmetic(VMWriter::Command::SUB);
			else if (op == '*')
				m_Writer.WriteCall("Math.multiply", 2);
			else if (op == '/')
				m_Writer.WriteCall("Math.divide", 2);
			else if (op == '&')
				m_Writer.WriteArithmetic(VMWriter::Command::AND);
			else if (op == '|')
				m_Writer.WriteArithmetic(VMWriter::Command::OR);
			else if (op == '>')
				m_Writer.WriteArithmetic(VMWriter::Command::GT);
			else if (op == '<')
				m_Writer.WriteArithmetic(VMWriter::Command::LT);
			else if (op == '=')
				m_Writer.WriteArithmetic(VMWriter::Command::EQ);
		}
	}
}

void CompilationEngine::CompileTerm()
{
	JackTokenType t = m_Tokenizer.TokenType();
	if (t == JackTokenType::INT_CONST)
	{
		m_Writer.WritePush(VMWriter::Segment::CONST, m_Tokenizer.IntVal());
		if (m_Tokenizer.HasMoreTokens())
			m_Tokenizer.Advance();
	}
	else if (t == JackTokenType::STRING_CONST)
	{
		const std::string& s = m_Tokenizer.StringVal();
		m_Writer.WritePush(VMWriter::Segment::CONST, s.size());
		m_Writer.WriteCall("String.new", 1);		// Pushes string base address onto stack.
		for (char c : s)
		{
			m_Writer.WritePush(VMWriter::Segment::CONST, c);	// Character to append.
			m_Writer.WriteCall("String.appendChar", 2);			// Returns string's 'this'.
		}
		if (m_Tokenizer.HasMoreTokens())
			m_Tokenizer.Advance();
	}
	else if (t == JackTokenType::KEYWORD)	// Jack constant (null, true, false, this).
	{
		const std::string kw = m_Tokenizer.KeyWord();
		if (kw == jack::J_NULL || kw == jack::FALSE)
			m_Writer.WritePush(VMWriter::Segment::CONST, 0);
		else if (kw == jack::TRUE)
		{
			m_Writer.WritePush(VMWriter::Segment::CONST, 1);
			m_Writer.WriteArithmetic(VMWriter::Command::NEG);
		}
		else if (kw == jack::THIS)
			m_Writer.WritePush(VMWriter::Segment::POINTER, 0);
		else
			throw JackTokenError(std::string("Unexpected keyword ") + kw);;
		CompileKeyWord(kw);
	}
	else if (t == JackTokenType::SYMBOL)
	{
		char c = m_Tokenizer.Symbol();
		if (c == '(')
		{ // Parenthesized expression.
			CompileSymbol('(');
			CompileExpression();
			CompileSymbol(')');
		}
		else if (c == '-' || c == '~')
		{ // Expression with unary operation.
			CompileSymbol(c);
			CompileTerm();
			VMWriter::Command cmd = VMWriter::Command::NEG;
			if (c == '~')
				 cmd = VMWriter::Command::NOT;
			m_Writer.WriteArithmetic(cmd);
		}
		else
			throw JackTokenError(std::string("Unexpected symbol ") + c);
	}
	else if (t == JackTokenType::IDENTIFIER)
	{
		std::string name = m_Tokenizer.Identifier();
		CompileIdentifier();
		t = m_Tokenizer.TokenType();
		char c = m_Tokenizer.Symbol();
		// Case 1: Accessing variable value.
		if (t != JackTokenType::SYMBOL || (c != '[' && c != '.' && c != '('))
			m_Writer.WritePush(NameToSgmt(name), m_ST.IndexOf(name));
		else                                      
		{// Case 2: Array entry, or subroutine call.
			if (c == '[')
			{	// Subcase 1: Array entry value.

				/* 
				* Array value computed below may be assigned to another
				* array entry, as in a[i] = a[i-1]. To be safe, preserve 'THAT'.
				*/
				m_Writer.WritePush(VMWriter::Segment::POINTER, 1);
				m_Writer.WritePop(VMWriter::Segment::TEMP, 0);
				ComputeArrayOffset(NameToSgmt(name), m_ST.IndexOf(name));
				m_Writer.WritePush(VMWriter::Segment::THAT, 0);	// Push entry value.
				m_Writer.WritePush(VMWriter::Segment::TEMP, 0);	// Restore old "that".
				m_Writer.WritePop(VMWriter::Segment::POINTER, 1);
			}
			else if (c == '.' || c == '(')	// Subcase 2: Subroutine call.
				CompileSubroutineCall(name);
		}
	}
	else
		throw JackTokenError(m_Tokenizer.Identifier());
}

int CompilationEngine::CompileExpressionList()
{
	JackTokenType t = m_Tokenizer.TokenType();
	int n_args = 0;
	if (t != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != ')')
	{
		CompileExpression();
		n_args++;
		while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == ',')
		{
			CompileSymbol(',');
			CompileExpression();
			n_args++;
		}
	}
	return n_args;
}

void CompilationEngine::CompileSymbol(char c)
{
	if (m_Tokenizer.TokenType() != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != c)
		throw JackTokenError(std::string("Expected ") + c);
	if (m_Tokenizer.HasMoreTokens())
		m_Tokenizer.Advance();
}

void CompilationEngine::CompileIdentifier()
{
	if (m_Tokenizer.TokenType() != JackTokenType::IDENTIFIER)
		throw JackTokenError("Expected identifier");
	const std::string& name = m_Tokenizer.Identifier();
	if (m_Tokenizer.HasMoreTokens())
		m_Tokenizer.Advance();
}

void CompilationEngine::CompileKeyWord(const std::string& kw)
{
	if (m_Tokenizer.KeyWord() != kw)
		throw JackTokenError(std::string("Expected ") + kw);
	if (m_Tokenizer.HasMoreTokens())
		m_Tokenizer.Advance();
}

const std::string CompilationEngine::CompileType()
{
	std::string type;
	switch (m_Tokenizer.TokenType())
	{
	case JackTokenType::IDENTIFIER:
		type = m_Tokenizer.Identifier();
		CompileIdentifier();
		return type;
	case JackTokenType::KEYWORD:
		type = m_Tokenizer.KeyWord();
		if (type != jack::INT && type != jack::CHAR && type != jack::BOOL)
			throw JackTokenError("Expected primitive type");
		CompileKeyWord(type);
		return type;
	default:
		throw JackTokenError("Expected type");
	}
}

void CompilationEngine::ComputeArrayOffset(VMWriter::Segment sgmt, int index)
{
	m_Writer.WritePush(sgmt, index);		// Base address of array.
	CompileSymbol('[');						// Compute array index, e.g. value 5.
	CompileExpression();
	CompileSymbol(']');						// End array index comp (on stack).
	m_Writer.WriteArithmetic(VMWriter::Command::ADD);	// E.g. a[5] or *(a+5).
	m_Writer.WritePop(VMWriter::Segment::POINTER, 1);
}

/* Subroutine call occurs in expression OR in do statement.
* Example 1: do Output.println("Hello");	static method.
* Example 2: do player.jump();				instance method on variable in scope.
* Example 3: do draw();						instance method on current class object.
* Example 4: let h = h + player.height();
*/
void CompilationEngine::CompileSubroutineCall(std::string& name)
{
	int n_args = 0;
	if (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == '.')
	{ // Case 1: name is a class name or variable name.
		CompileSymbol('.');
		const std::string& method_name = m_Tokenizer.Identifier();
		CompileIdentifier();
		if (m_ST.KindOf(name) != SymbolTable::Kind::NONE)
		{ // Subcase 1: name is a class name
			m_Writer.WritePush(NameToSgmt(name), m_ST.IndexOf(name));
			n_args++;	// Variable that method is called on is first argument.
			name = m_ST.TypeOf(name) + "." + method_name;
		}
		else // Subcase 2: name is a class name.
			name = name + "." + method_name;
	}
	else
	{	// Case 2: name is an instance method.
		m_Writer.WritePush(VMWriter::Segment::POINTER, 0);
		n_args++;
		name = m_ClassName + "." + name;
	}
	CompileSymbol('(');
	n_args += CompileExpressionList();
	CompileSymbol(')');
	m_Writer.WriteCall(name, n_args);
}

VMWriter::Segment CompilationEngine::NameToSgmt(const std::string& name)
{
	switch (m_ST.KindOf(name))
	{
	case SymbolTable::Kind::VAR:
		return VMWriter::Segment::LOCAL;
	case SymbolTable::Kind::ARG:
		return VMWriter::Segment::ARG;
	case SymbolTable::Kind::STATIC:
		return VMWriter::Segment::STATIC;
	case SymbolTable::Kind::FIELD:
		return VMWriter::Segment::THIS;
	default:
		throw JackIdentifierError(name);
	}
}

} // namespace jack