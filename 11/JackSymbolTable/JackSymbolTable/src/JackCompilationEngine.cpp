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
		PrintIdentifier("class", false);

		// Compile '{'
		PrintSymbol();
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
		PrintSymbol();
	}
	*m_pOutFile << "</class>\n";
}

void JackCompilationEngine::CompileClassVarDec()
{
	*m_pOutFile << "<classVarDec>\n";

	// Prepare an identifier.
	// field or static.
	if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::STATIC)
	{
		PrintKeyWord("static", JackTokenizer::KeyWord::STATIC);
		m_CurrentIdentifier.kind = SymbolTable::Kind::STATIC;
	}
	else
	{
		PrintKeyWord("field", JackTokenizer::KeyWord::FIELD);
		m_CurrentIdentifier.kind = SymbolTable::Kind::FIELD;
	}

	// Variable type -- keyword or identifier, index.
	PrintType();

	// Variable name -- identifier
	std::string category = (m_CurrentIdentifier.kind == SymbolTable::Kind::STATIC ? "static " : "field");
	PrintIdentifier(category, true);

	// Possibly more variable names within same line.
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == ',')
	{
		// Print ','
		PrintSymbol();
		PrintIdentifier(category, true);
	}

	// Print ';'
	PrintSymbol();

	*m_pOutFile << "</classVarDec>\n";
}

void JackCompilationEngine::CompileSubroutine()
{
	*m_pOutFile << "<subroutineDec>\n";
	m_SymbolTable.StartSubroutine();
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
	PrintIdentifier("subroutine", false);

	// Print '('
	PrintSymbol();
	CompileParameterList();
	// Print ')'
	PrintSymbol();

	//subroutineBody
	*m_pOutFile << "<subroutineBody>\n";
	// Print('{')
	PrintSymbol();
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::VAR)
		CompileVarDec();
	CompileStatements();
	// Print('}')
	PrintSymbol();
	*m_pOutFile << "</subroutineBody>\n";

	*m_pOutFile << "</subroutineDec>\n";
}

void JackCompilationEngine::CompileParameterList()
{
	*m_pOutFile << "<parameterList>\n";
	if (m_pTokenizer->GetTokenType() != JackTokenizer::TokenType::SYMBOL ||
		m_pTokenizer->GetSymbol() != ')')
	{
		m_CurrentIdentifier.kind = SymbolTable::Kind::ARG;
		PrintType();
		PrintIdentifier("argument", true);

		// possible list of comman-separated variable names in addition to the first.
		while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
			m_pTokenizer->GetSymbol() == ',')
		{
			// Print(',')
			PrintSymbol();
			PrintType();
			PrintIdentifier("argument", true);
		}
	}
	*m_pOutFile << "</parameterList>\n";
}

void JackCompilationEngine::CompileVarDec()
{
	// Create an Identifier struct.
	// Assign an index?
	*m_pOutFile << "<varDec>\n";
	// Assign its VAR to its kind.

	PrintKeyWord("var", JackTokenizer::KeyWord::VAR);

	// Assign the type to the struct.
	PrintType();

	PrintIdentifier("var", true);
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == ',')
	{
		// Print(',')
		PrintSymbol();
		PrintIdentifier("var", true);
	}
	// Print(';')
	PrintSymbol();
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

	PrintSubroutineCall();
	// Print ';'
	PrintSymbol();
	*m_pOutFile << "</doStatement>\n";
}

void JackCompilationEngine::CompileLet()
{
	*m_pOutFile << "<letStatement>\n";
	PrintKeyWord("let", JackTokenizer::KeyWord::LET);

	// Variable name.
	m_CurrentIdentifier.name = m_pTokenizer->GetIdentifier();
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
		PrintIdentifier(m_SymbolTable.TypeOf(m_CurrentIdentifier.name), false);

	// Possibly an array access.
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == '[')
	{
		// Print '['
		PrintSymbol();
		CompileExpression();
		// Print(']')
		PrintSymbol();
	}
	// Print('=')
	PrintSymbol();

	// New value of variable is right-hand-side expression.
	CompileExpression();
	// Print(';')
	PrintSymbol();

	*m_pOutFile << "</letStatement>\n";
}

void JackCompilationEngine::CompileWhile()
{
	*m_pOutFile << "<whileStatement>\n";
	PrintKeyWord("while", JackTokenizer::KeyWord::WHILE);
	// Print('(')
	PrintSymbol();

	// Boolean expression to test while loop.
	CompileExpression();

	// Print(')')
	PrintSymbol();
	// Print('{')
	PrintSymbol();

	// While loop body.
	CompileStatements();

	// Print('}')
	PrintSymbol();
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
	// Print(';')
	PrintSymbol();
	*m_pOutFile << "</returnStatement>\n";
}

void JackCompilationEngine::CompileIf()
{
	*m_pOutFile << "<ifStatement>\n";
	PrintKeyWord("if", JackTokenizer::KeyWord::IF);
	// Print('(')
	PrintSymbol();
	// Expression to test if statement.
	CompileExpression();
	// Print(')')
	PrintSymbol();
	// Print('{')
	PrintSymbol();

	// Body of the if statement
	CompileStatements();
	// Print('}')
	PrintSymbol();
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::ELSE)
	{
		PrintKeyWord("else", JackTokenizer::KeyWord::ELSE);
		// Print('{')
		PrintSymbol();
		// Body of the else suit.
		CompileStatements();
		// Print('}')
		PrintSymbol();
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
			PrintSymbol();
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
			// Print '('
			PrintSymbol();
			CompileExpression();
			// Print ')'
			PrintSymbol();
		}
		else if (m_pTokenizer->GetSymbol() == '-' || m_pTokenizer->GetSymbol() == '~')
		{
			// Print '-' or '~'
			PrintSymbol();
			CompileTerm();
		}
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		// Either an variable, an array entry, or a subroutine call.
		m_CurrentIdentifier.name = m_pTokenizer->GetIdentifier();
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();

		char sym = m_pTokenizer->GetSymbol();
		// It's a variable name and it's an array.
		if (sym == '[')
		{
			// Identifier for array name.
			*m_pOutFile << "<identifier category=\"" << m_SymbolTable.TypeOf(m_CurrentIdentifier.name) << "\" "
				<< "context=\"used\" "
				<< "scopedVar=\"true\" "
				// Index is meaningless if it's not a scoped variable.
				<< "index=\"" << m_SymbolTable.IndexOf(m_CurrentIdentifier.name) << "\" > "
				<< m_CurrentIdentifier.name << " </identifier>\n";
			// Print('[')
			PrintSymbol();
			CompileExpression();
			// Print(']')
			PrintSymbol();
		}
		else if (sym == '.' || sym == '(')
			PrintSubroutineCall();
		else
			*m_pOutFile << "<identifier category=\"" << m_SymbolTable.TypeOf(m_CurrentIdentifier.name) << "\" "
			<< "context=\"used\" "
			<< "scopedVar=\"true\" "
			// Index is meaningless if it's not a scoped variable.
			<< "index=\"" << m_SymbolTable.IndexOf(m_CurrentIdentifier.name) << "\" > "
			<< m_CurrentIdentifier.name << " </identifier>\n";
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
			// Print(',')
			PrintSymbol();
			CompileExpression();
		}
	}
	*m_pOutFile << "</expressionList>\n";
}

void JackCompilationEngine::PrintSymbol()
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL)
	{
		if (m_pTokenizer->GetSymbol() == '<')
			*m_pOutFile << "<symbol> &lt; </symbol>\n";
		else if (m_pTokenizer->GetSymbol() == '>')
			*m_pOutFile << "<symbol> &gt; </symbol>\n";
		else if (m_pTokenizer->GetSymbol() == '&')
			*m_pOutFile << "<symbol> &amp; </symbol>\n";
		else
			*m_pOutFile << "<symbol> " << m_pTokenizer->GetSymbol() << " </symbol>\n";
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

void JackCompilationEngine::PrintIdentifier(std::string category, bool isBeingDefined)
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		// Save the identifier.
		m_CurrentIdentifier.name = m_pTokenizer->GetIdentifier();

		// Define variable symbol, if necessary.
		if (isBeingDefined)
			m_SymbolTable.Define(m_CurrentIdentifier.name, m_CurrentIdentifier.type, m_CurrentIdentifier.kind);

		// Output XML tag.
		*m_pOutFile << "<identifier category=\"" << category
			<< "\" context=\"" << (isBeingDefined ? "defined" : "used")
			<< "\" scopedVar=\"" << (m_SymbolTable.KindOf(m_CurrentIdentifier.name) == SymbolTable::Kind::NONE ? "false" : "true")
			// Index is meaningless if it's not a scoped variable.
			<< "\" index=\"" << m_SymbolTable.IndexOf(m_CurrentIdentifier.name) << "\"> "
			<< m_CurrentIdentifier.name << " </identifier>\n";

		// Move on to next token.
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
		if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::INT)
		{
			*m_pOutFile << "<keyword> int </keyword>\n";
			m_CurrentIdentifier.type = "int";
		}
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::CHAR)
		{
			*m_pOutFile << "<keyword> char </keyword>\n";
			m_CurrentIdentifier.type = "char";
		}
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::BOOLEAN)
		{
			*m_pOutFile << "<keyword> boolean </keyword>\n";
			m_CurrentIdentifier.type = "int";
		}
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	else
		// Variable of type class, and it's a type, so it's not a scoped variable. The class is being used.
		PrintIdentifier("class", false);
}

void JackCompilationEngine::PrintSubroutineCall()
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		m_CurrentIdentifier.name = m_pTokenizer->GetIdentifier();
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();

		if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
			m_pTokenizer->GetSymbol() == '.')
		{
			if (m_SymbolTable.KindOf(m_CurrentIdentifier.name) == SymbolTable::Kind::NONE)
			{
				*m_pOutFile << "<identifier category=\"class\" "
					<< "context=\"used\" "
					<< "scopedVar=\"false\" "
					// Index is meaningless if it's not a scoped variable.
					<< "index=\"-1\" > "
					<< m_CurrentIdentifier.name << " </identifier>\n";
			}
			else
			{
				std::string category;
				switch (m_SymbolTable.KindOf(m_CurrentIdentifier.name))
				{
					case SymbolTable::Kind::STATIC:
						category = "static";
						break;
					case SymbolTable::Kind::VAR:
						category = "var";
						break;
					case SymbolTable::Kind::FIELD:
						category = "field";
						break;
					case SymbolTable::Kind::ARG:
						category = "argument";
						break;
					default:
						break;
				}
				*m_pOutFile << "<identifier category=\"" << category << "\" "
					<< "context=\"used\" "
					<< "scopedVar=\"true\" "
					<< "index=\"" << m_SymbolTable.IndexOf(m_CurrentIdentifier.name) << "\" > "
					<< m_CurrentIdentifier.name << " </identifier>\n";
			}
			PrintSymbol();
			PrintIdentifier("subroutine", false);
		}
		// subroutine call on 'this' object.
		else
			*m_pOutFile << "<identifier category=\"subroutine\" "
			<< "context=\"used\" "
			<< "scopedVar=\"false\" "
			// Index is meaningless if it's not a scoped variable.
			<< "index=\"-1\" > "
			<< m_CurrentIdentifier.name << " </identifier>\n";
	}
	// Print '('
	PrintSymbol();
	// Argument expressions.
	CompileExpressionList();
	// Print ')'
	PrintSymbol();
}