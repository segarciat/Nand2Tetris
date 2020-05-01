#pragma once

#include "JackCompilationEngine.h"

JackCompilationEngine::JackCompilationEngine(std::string sourceInName, std::string outName)
{
	m_pTokenizer = new JackTokenizer(sourceInName);
	m_pOutFile = new std::ofstream(outName);
}

JackCompilationEngine::~JackCompilationEngine()
{
	delete m_pTokenizer;
	m_pOutFile->close();
	delete m_pOutFile;
}

void JackCompilationEngine::CompileClass()
{
	*m_pOutFile << "<class>\n";
	if (m_pTokenizer->HasMoreTokens())
	{
		m_pTokenizer->Advance();
		// Compile class keyword
		PrintKeyWord("class", JackTokenizer::KeyWord::CLASS);

		// Compile class name (identifier)
		PrintIdentifier();

		// Compile '{'
		PrintSymbol('{');
		// Compile static/field variable declarations, if any.
		while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
			(m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::STATIC ||
				m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::FIELD))
			CompileClassVarDec();

		// Compile subroutine declarations, if any.
		while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
			(m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::CONSTRUCTOR ||
				m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::METHOD ||
				m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::FUNCTION))
			CompileSubroutine();

		// Compile '}'
		PrintSymbol('}');
	}
	*m_pOutFile << "</class>\n";
}

void JackCompilationEngine::CompileClassVarDec()
{
	*m_pOutFile << "<classVarDec>\n";
	// field or static.
	if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::STATIC)
		PrintKeyWord("static", JackTokenizer::KeyWord::STATIC);
	else
		PrintKeyWord("field", JackTokenizer::KeyWord::FIELD);

	// Variable type -- keyword or identifier.
	PrintType();

	// Variable name -- identifier
	PrintIdentifier();

	// Possibly more variable names within same line.
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == ',')
	{
		PrintSymbol(',');
		PrintIdentifier();
	}

	// Print ';'
	PrintSymbol(';');

	*m_pOutFile << "</classVarDec>\n";
}

void JackCompilationEngine::CompileSubroutine()
{
	*m_pOutFile << "<subroutineDec>\n";
	// Subroutine keyword
	if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::CONSTRUCTOR)
		PrintKeyWord("constructor", JackTokenizer::KeyWord::CONSTRUCTOR);
	else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::METHOD)
		PrintKeyWord("method", JackTokenizer::KeyWord::METHOD);
	else
		PrintKeyWord("function", JackTokenizer::KeyWord::FUNCTION);

	// print subroutine type - void or a type
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::VOID)
		PrintKeyWord("void", JackTokenizer::KeyWord::VOID);
	else
		PrintType();

	// print subroutine name
	PrintIdentifier();

	PrintSymbol('(');
	CompileParameterList();
	PrintSymbol(')');

	//subroutineBody
	*m_pOutFile << "<subroutineBody>\n";
	PrintSymbol('{');
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::VAR)
		CompileVarDec();
	CompileStatements();
	PrintSymbol('}');
	*m_pOutFile << "</subroutineBody>\n";

	*m_pOutFile << "</subroutineDec>\n";
}

void JackCompilationEngine::CompileParameterList()
{
	*m_pOutFile << "<parameterList>\n";
	if (m_pTokenizer->GetTokenType() != JackTokenizer::TokenType::SYMBOL ||
		m_pTokenizer->GetSymbol() != ')')
	{
		PrintType();
		PrintIdentifier();

		// possible list of comman-separated variable names in addition to the first.
		while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
			m_pTokenizer->GetSymbol() == ',')
		{
			PrintSymbol(',');
			PrintType();
			PrintIdentifier();
		}
	}
	*m_pOutFile << "</parameterList>\n";
}

void JackCompilationEngine::CompileVarDec()
{
	*m_pOutFile << "<varDec>\n";
	PrintKeyWord("var", JackTokenizer::KeyWord::VAR);
	PrintType();
	PrintIdentifier();
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == ',')
	{
		PrintSymbol(',');
		PrintIdentifier();
	}
	PrintSymbol(';');
	*m_pOutFile << "</varDec>\n";
}

void JackCompilationEngine::CompileStatements()
{
	*m_pOutFile << "<statements>\n";
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD)
	{
		if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::DO)
			CompileDo();
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::LET)
			CompileLet();
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::WHILE)
			CompileWhile();
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::RETURN)
			CompileReturn();
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::IF)
			CompileIf();
		else
			break;
	}
	*m_pOutFile << "</statements>\n";
}

void JackCompilationEngine::CompileDo()
{
	*m_pOutFile << "<doStatement>\n";
	PrintKeyWord("do", JackTokenizer::KeyWord::DO);

	// Name of subroutine being called OR the class name OR object instance name.
	PrintIdentifier();
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == '.')
	{
		// Member access operator.
		PrintSymbol('.');
		// Name of the subroutine name accessed through '.' operator.
		PrintIdentifier();
	}
	PrintSymbol('(');
	CompileExpressionList();
	PrintSymbol(')');
	PrintSymbol(';');
	*m_pOutFile << "</doStatement>\n";
}

void JackCompilationEngine::CompileLet()
{
	*m_pOutFile << "<letStatement>\n";
	PrintKeyWord("let", JackTokenizer::KeyWord::LET);

	// Variable name.
	PrintIdentifier();

	// Possibly an array access.
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == '[')
	{
		PrintSymbol('[');
		CompileExpression();
		PrintSymbol(']');
	}

	PrintSymbol('=');

	// New value of variable is right-hand-side expression.
	CompileExpression();
	PrintSymbol(';');

	*m_pOutFile << "</letStatement>\n";
}

void JackCompilationEngine::CompileWhile()
{
	*m_pOutFile << "<whileStatement>\n";
	PrintKeyWord("while", JackTokenizer::KeyWord::WHILE);
	PrintSymbol('(');

	// Boolean expression to test while loop.
	CompileExpression();

	PrintSymbol(')');
	PrintSymbol('{');

	// While loop body.
	CompileStatements();

	PrintSymbol('}');
	*m_pOutFile << "</whileStatement>\n";
}

void JackCompilationEngine::CompileReturn()
{
	*m_pOutFile << "<returnStatement>\n";
	PrintKeyWord("return", JackTokenizer::KeyWord::RETURN);
	// Check if there is indeed an expression.
	if (m_pTokenizer->GetTokenType() != JackTokenizer::TokenType::SYMBOL ||
		m_pTokenizer->GetSymbol() != ';')
		CompileExpression();
	PrintSymbol(';');
	*m_pOutFile << "</returnStatement>\n";
}

void JackCompilationEngine::CompileIf()
{
	*m_pOutFile << "<ifStatement>\n";
	PrintKeyWord("if", JackTokenizer::KeyWord::IF);
	PrintSymbol('(');
	// Expression to test if statement.
	CompileExpression();
	PrintSymbol(')');
	PrintSymbol('{');

	// Body of the if statement
	CompileStatements();
	PrintSymbol('}');
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::ELSE)
	{
		PrintKeyWord("else", JackTokenizer::KeyWord::ELSE);
		PrintSymbol('{');
		// Body of the else suit.
		CompileStatements();
		PrintSymbol('}');
	}
	*m_pOutFile << "</ifStatement>\n";
}

void JackCompilationEngine::CompileExpression()
{
	*m_pOutFile << "<expression>\n";
	CompileTerm();
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL)
	{
		char sym = m_pTokenizer->GetSymbol();
		if (sym == '+' || sym == '-' || sym == '*' || sym == '/' || sym == '&' ||
			sym == '|' || sym == '<' || sym == '>' || sym == '=')
		{
			PrintSymbol(sym);
			CompileTerm();
		}
		else
			break;
	}
	*m_pOutFile << "</expression>\n";
}

void JackCompilationEngine::CompileTerm()
{
	*m_pOutFile << "<term>\n";
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::STRING_CONST)
	{
		*m_pOutFile << "<stringConstant> " << m_pTokenizer->GetStringVal() << " </stringConstant>\n";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::INT_CONST)
	{
		*m_pOutFile << "<integerConstant> " << m_pTokenizer->GetIntVal() << " </integerConstant>\n";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD)
	{
		if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::TRUE)
			PrintKeyWord("true", JackTokenizer::KeyWord::TRUE);
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::FALSE)
			PrintKeyWord("false", JackTokenizer::KeyWord::FALSE);
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::JNULL)
			PrintKeyWord("null", JackTokenizer::KeyWord::JNULL);
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::THIS)
			PrintKeyWord("this", JackTokenizer::KeyWord::THIS);
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL)
	{
		if (m_pTokenizer->GetSymbol() == '(')
		{
			PrintSymbol('(');
			CompileExpression();
			PrintSymbol(')');
		}
		else if (m_pTokenizer->GetSymbol() == '-' || m_pTokenizer->GetSymbol() == '~')
		{
			PrintSymbol(m_pTokenizer->GetSymbol());
			CompileTerm();
		}
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		// Either an variable, an array entry, or a subroutine call.
		PrintIdentifier();
		char sym = m_pTokenizer->GetSymbol();
		// Case 1: it's an array entry.
		if (sym == '[')
		{
			PrintSymbol('[');
			CompileExpression();
			PrintSymbol(']');
		}
		// Case 2: It's a subroutine call.
		else if (sym == '.' || sym == '(')
		{
			// Particularly, calling it on a class or on an object instance.
			if (sym == '.')
			{
				// Member access operator.
				PrintSymbol('.');
				// Name of the subroutine name accessed through '.' operator.
				PrintIdentifier();
			}
			PrintSymbol('(');
			CompileExpressionList();
			PrintSymbol(')');
		}
	}
	*m_pOutFile << "</term>\n";
}

void JackCompilationEngine::CompileExpressionList()
{

	*m_pOutFile << "<expressionList>\n";
	if (m_pTokenizer->GetTokenType() != JackTokenizer::TokenType::SYMBOL ||
		m_pTokenizer->GetSymbol() != ')')
	{
		CompileExpression();
		while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
			m_pTokenizer->GetSymbol() == ',')
		{
			PrintSymbol(',');
			CompileExpression();
		}
	}
	*m_pOutFile << "</expressionList>\n";
}

void JackCompilationEngine::PrintSymbol(char sym)
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == sym)
	{
		if (sym == '<')
			*m_pOutFile << "<symbol> &lt; </symbol>\n";
		else if (sym == '>')
			*m_pOutFile << "<symbol> &gt; </symbol>\n";
		else if (sym == '&')
			*m_pOutFile << "<symbol> &amp; </symbol>\n";
		else
			*m_pOutFile << "<symbol> " << sym << " </symbol>\n";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
}

void JackCompilationEngine::PrintKeyWord(std::string keyWordString, JackTokenizer::KeyWord keyWordToken)
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == keyWordToken)
	{
		*m_pOutFile << "<keyword> " << keyWordString << " </keyword>\n";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
}

void JackCompilationEngine::PrintIdentifier()
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		*m_pOutFile << "<identifier> " << m_pTokenizer->GetIdentifier() << " </identifier>\n";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
}

void JackCompilationEngine::PrintType()
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		(m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::INT ||
			m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::CHAR ||
			m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::BOOLEAN))
	{
		// Print th
		if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::INT)
			*m_pOutFile << "<keyword> int </keyword>\n";
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::CHAR)
			*m_pOutFile << "<keyword> char </keyword>\n";
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::BOOLEAN)
			*m_pOutFile << "<keyword> boolean </keyword>\n";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		*m_pOutFile << "<identifier> " << m_pTokenizer->GetIdentifier() << " </identifier>\n";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
}