#include "CompilationEngine.h"
#include "JackTokenError.h"


CompilationEngine::CompilationEngine(const std::filesystem::path& srcPath)
	:m_Tokenizer{ srcPath }
{
	m_Ofs.open(srcPath.stem().string() + g_TARGET_EXT);
	if (!m_Ofs)
		throw std::ofstream::failure("Problem encountered while creating " + srcPath.stem().string());
}

void CompilationEngine::CompileClass()
{
	if (m_Tokenizer.HasMoreTokens())
	{
		m_Tokenizer.Advance();
		if (m_Tokenizer.TokenType() != JackTokenType::KEYWORD || m_Tokenizer.KeyWord() != "class")
			throw JackTokenError("Expected class");
		m_Ofs << "<class>\n";
		CompileKeyWord("class");
		CompileIdentifier();
		CompileSymbol('{');
		// All class variables must be declared at start of class.
		while (m_Tokenizer.TokenType() == JackTokenType::KEYWORD &&
			(m_Tokenizer.KeyWord() == "static" || m_Tokenizer.KeyWord() == "field"))
			CompileClassVarDec();
		// All (if any) subroutine declarations must follow class variable declarations.
		while (m_Tokenizer.TokenType() == JackTokenType::KEYWORD &&
			(m_Tokenizer.KeyWord() == "method" || m_Tokenizer.KeyWord() == "constructor" ||
				m_Tokenizer.KeyWord() == "function"))
			CompileSubroutine();
		CompileSymbol('}');
		m_Ofs << "</class>\n";
	}
}

void CompilationEngine::CompileClassVarDec()
{
	m_Ofs << "<classVarDec>\n";
	CompileKeyWord(m_Tokenizer.KeyWord());
	CompileType();
	CompileIdentifier();
	// Possibly declaring multiple variables at once.
	while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == ',')
	{
		CompileSymbol(',');
		CompileIdentifier();
	}
	CompileSymbol(';');
	m_Ofs << "</classVarDec>\n";
}

void CompilationEngine::CompileSubroutine()
{
	m_Ofs << "<subroutineDec>\n";
	// Subroutine header.
	CompileKeyWord(m_Tokenizer.KeyWord());
	// Return type may be void, primitive-type, or user-defined type.
	if (m_Tokenizer.TokenType() == JackTokenType::KEYWORD && m_Tokenizer.KeyWord() == "void")
		CompileKeyWord("void");
	else
		CompileType();
	CompileIdentifier();
	CompileSymbol('(');
	CompileParameterList();
	CompileSymbol(')');

	// Subroutine body.
	m_Ofs << "<subroutineBody>\n";
	CompileSymbol('{');
	// All variable declarations must appear at start of subroutine.
	while (m_Tokenizer.TokenType() == JackTokenType::KEYWORD && m_Tokenizer.KeyWord() == "var")
		CompileVarDec();
	CompileStatements();
	CompileSymbol('}');
	m_Ofs << "</subroutineBody>\n";
	m_Ofs << "</subroutineDec>\n";

}

void CompilationEngine::CompileParameterList()
{
	m_Ofs << "<parameterList>\n";
	// Possibly empty parameter list.
	if (m_Tokenizer.TokenType() != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != ')')
	{
		CompileType();
		CompileIdentifier();
		// If non-empty, could have more than one parameter.
		while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == ',')
		{
			CompileSymbol(',');
			CompileType();
			CompileIdentifier();
		}
	}
	m_Ofs << "</parameterList>\n";
}

void CompilationEngine::CompileVarDec()
{
	m_Ofs << "<varDec>\n";
	CompileKeyWord("var");
	CompileType();
	CompileIdentifier();
	// Possibly more than one variable in same declaration.
	while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == ',')
	{
		CompileSymbol(',');
		CompileIdentifier();
	}
	CompileSymbol(';');
	m_Ofs << "</varDec>\n";
}

void CompilationEngine::CompileStatements()
{
	m_Ofs << "<statements>\n";
	while (m_Tokenizer.TokenType() == JackTokenType::KEYWORD)
	{
		const std::string kw = m_Tokenizer.KeyWord();
		if (kw == "do")
			CompileDo();
		else if (kw == "let")
			CompileLet();
		else if (kw == "while")
			CompileWhile();
		else if (kw == "return")
			CompileReturn();
		else if (kw == "if")
			CompileIf();
		else
			throw JackTokenError(std::string("Unexpected keyword ") + kw);
	}
	m_Ofs << "</statements>\n";
}

void CompilationEngine::CompileDo()
{
	m_Ofs << "<doStatement>\n";
	CompileKeyWord("do");
	// Subroutine may be a method of this class, or of another class.
	CompileIdentifier();
	if (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == '.')
	{
		CompileSymbol('.');
		CompileIdentifier();
	}
	CompileSymbol('(');
	CompileExpressionList();
	CompileSymbol(')');
	CompileSymbol(';');
	m_Ofs << "</doStatement>\n";
}

void CompilationEngine::CompileLet()
{
	m_Ofs << "<letStatement>\n";
	CompileKeyWord("let");
	CompileIdentifier();
	if (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == '[')
	{
		CompileSymbol('[');
		CompileExpression();
		CompileSymbol(']');
	}
	CompileSymbol('=');
	CompileExpression();
	CompileSymbol(';');
	m_Ofs << "</letStatement>\n";
}

void CompilationEngine::CompileWhile()
{
	m_Ofs << "<whileStatement>\n";
	CompileKeyWord("while");
	CompileSymbol('(');
	CompileExpression();
	CompileSymbol(')');
	CompileSymbol('{');
	CompileStatements();
	CompileSymbol('}');
	m_Ofs << "</whileStatement>\n";
}

void CompilationEngine::CompileReturn()
{
	m_Ofs << "<returnStatement>\n";
	CompileKeyWord("return");
	// Possibl empty return statement.
	if (m_Tokenizer.TokenType() != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != ';')
		CompileExpression();
	CompileSymbol(';');
	m_Ofs << "</returnStatement>\n";
}

void CompilationEngine::CompileIf()
{
	m_Ofs << "<ifStatement>\n";
	CompileKeyWord("if");
	CompileSymbol('(');
	CompileExpression();
	CompileSymbol(')');
	CompileSymbol('{');
	CompileStatements();
	CompileSymbol('}');
	// Optional else statement following if.
	if (m_Tokenizer.TokenType() == JackTokenType::KEYWORD && m_Tokenizer.KeyWord() == "else")
	{
		CompileKeyWord("else");
		CompileSymbol('{');
		CompileStatements();
		CompileSymbol('}');
	}
	m_Ofs << "</ifStatement>\n";
}

void CompilationEngine::CompileExpression()
{
	m_Ofs << "<expression>\n";
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
		}
	}
	m_Ofs << "</expression>\n";
}

void CompilationEngine::CompileTerm()
{
	m_Ofs << "<term>\n";
	JackTokenType t = m_Tokenizer.TokenType();
	if (t == JackTokenType::INT_CONST)
	{
		m_Ofs << "<integerConstant> " << m_Tokenizer.IntVal() << " </integerConstant>\n";
		if (m_Tokenizer.HasMoreTokens())
			m_Tokenizer.Advance();
	}
	else if (t == JackTokenType::STRING_CONST)
	{
		m_Ofs << "<stringConstant> " << m_Tokenizer.StringVal() << " </stringConstant>\n";
		if (m_Tokenizer.HasMoreTokens())
			m_Tokenizer.Advance();
	}
	else if (t == JackTokenType::KEYWORD)
	{
		const std::string kw = m_Tokenizer.KeyWord();
		if (kw != "null" && kw != "true" && kw != "false" && kw != "this")
			throw JackTokenError(std::string("Unexpected keyword ") + kw);
		CompileKeyWord(kw);
	}
	else if (t == JackTokenType::SYMBOL)
	{
		char c = m_Tokenizer.Symbol();
		if (c == '(')
		{
			CompileSymbol('(');
			CompileExpression();
			CompileSymbol(')');
		}
		else if (c == '-' || c == '~')
		{
			CompileSymbol(c);
			CompileTerm();
		}
		else
			throw JackTokenError(std::string("Unexpected symbol ") + c);
	}
	else if (t == JackTokenType::IDENTIFIER)
	{
		CompileIdentifier();
		t = m_Tokenizer.TokenType();
		if (t == JackTokenType::SYMBOL)
		{
			char c = m_Tokenizer.Symbol();
			if (c == '[')						// Array entry
			{
				CompileSymbol('[');
				CompileExpression();
				CompileSymbol(']');
			}
			else if (c == '.' || c == '(')		// Subroutine call.
			{
				if (c == '.')
				{
					CompileSymbol('.');
					CompileIdentifier();
				}
				CompileSymbol('(');
				CompileExpressionList();
				CompileSymbol(')');
			}
		}
	}
	else
		throw JackTokenError(m_Tokenizer.Identifier());
	m_Ofs << "</term>\n";
}

void CompilationEngine::CompileExpressionList()
{
	m_Ofs << "<expressionList>\n";
	JackTokenType t = m_Tokenizer.TokenType();
	if (t != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != ')')
	{
		CompileExpression();
		while (m_Tokenizer.TokenType() == JackTokenType::SYMBOL && m_Tokenizer.Symbol() == ',')
		{
			CompileSymbol(',');
			CompileExpression();
		}
	}
	m_Ofs << "</expressionList>\n";
}

void CompilationEngine::CompileSymbol(char c)
{
	if (m_Tokenizer.TokenType() != JackTokenType::SYMBOL || m_Tokenizer.Symbol() != c)
		throw JackTokenError(std::string("Expected ") + c);
	if (c == '<')
		m_Ofs << "<symbol> &lt; </symbol>\n";
	else if (c == '>')
		m_Ofs << "<symbol> &gt; </symbol>\n";
	else if (c == '&')
		m_Ofs << "<symbol> &amp; </symbol>\n";
	else
		m_Ofs << "<symbol> " << c << " </symbol>\n";
	if (m_Tokenizer.HasMoreTokens())
		m_Tokenizer.Advance();
}

void CompilationEngine::CompileIdentifier()
{
	if (m_Tokenizer.TokenType() != JackTokenType::IDENTIFIER)
		throw JackTokenError("Expected identifier");
	m_Ofs << "<identifier> " << m_Tokenizer.Identifier() << " </identifier>\n";
	if (m_Tokenizer.HasMoreTokens())
		m_Tokenizer.Advance();
}

void CompilationEngine::CompileKeyWord(const std::string& kw)
{
	if (m_Tokenizer.KeyWord() != kw)
		throw JackTokenError(std::string("Expected ") + kw);
	m_Ofs << "<keyword> " << kw << " </keyword>\n";
	if (m_Tokenizer.HasMoreTokens())
		m_Tokenizer.Advance();
}

void CompilationEngine::CompileType()
{
	JackTokenType type = m_Tokenizer.TokenType();
	if (type == JackTokenType::IDENTIFIER)
		CompileIdentifier();
	else if (type == JackTokenType::KEYWORD)
	{
		const std::string kw = m_Tokenizer.KeyWord();
		if (kw != "int" && kw != "char" && kw != "boolean")
			throw JackTokenError("Expected primitive type");
		CompileKeyWord(kw);
	}
	else
		throw JackTokenError("Expected type");
}