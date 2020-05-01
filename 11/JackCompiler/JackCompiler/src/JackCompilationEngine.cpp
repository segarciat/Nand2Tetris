#pragma once

#include "JackCompilationEngine.h"

JackCompilationEngine::JackCompilationEngine(std::string& sourceInName, std::string& outName):
	m_LabelCounter(0), m_ClassName("")
{
	m_pTokenizer = new JackTokenizer(sourceInName);
	m_pCodeWriter = new VMWriter(outName);
}

JackCompilationEngine::~JackCompilationEngine()
{
	delete m_pTokenizer;
	delete m_pCodeWriter;
}

void JackCompilationEngine::CompileClass()
{
	if (m_pTokenizer->HasMoreTokens())
	{
		m_pTokenizer->Advance();
		// Compile class keyword
		PrintKeyWord("class", JackTokenizer::KeyWord::CLASS);

		// Get the classname.
		PrintIdentifier();
		m_ClassName = m_CurrentIdentifier.name;


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
}

void JackCompilationEngine::CompileClassVarDec()
{
	// Set the kind of class variable.
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

	// Sets the current identifier's type to a primitive or class type.
	PrintType();

	// Variable name -- identifier
	PrintIdentifier();

	// First class variable.
	m_SymbolTable.Define(m_CurrentIdentifier.name, m_CurrentIdentifier.type, m_CurrentIdentifier.kind);


	// Possibly more variable names within same line.
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == ',')
	{
		// Print ','
		PrintSymbol();
		// Variable name.
		PrintIdentifier();
		m_SymbolTable.Define(m_CurrentIdentifier.name, m_CurrentIdentifier.type, m_CurrentIdentifier.kind);
	}
	// Print ';'
	PrintSymbol();
}

void JackCompilationEngine::CompileSubroutine()
{
	// Clear out the symbol table.
	m_SymbolTable.StartSubroutine();

	// Subroutine keyword
	bool isMethod = false;
	bool isConstructor = false;
	if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::CONSTRUCTOR)
	{
		PrintKeyWord("constructor", JackTokenizer::KeyWord::CONSTRUCTOR);
		isConstructor = true;
	}
	else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::METHOD)
	{
		PrintKeyWord("method", JackTokenizer::KeyWord::METHOD);
		// 'this' is always the first argument in a method.
		m_SymbolTable.Define("this", m_ClassName, SymbolTable::Kind::ARG);
		isMethod = true;
	}
	else
		PrintKeyWord("function", JackTokenizer::KeyWord::FUNCTION);

	// print subroutine type - void or a type
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::VOID)
		PrintKeyWord("void", JackTokenizer::KeyWord::VOID);
	else
		PrintType();

	// Fully quallified named on the function.
	PrintIdentifier();
	std::string functionName = m_ClassName + "." + m_CurrentIdentifier.name;

	// Print '('
	PrintSymbol();
	// Defines function parameters in symbol table.
	CompileParameterList();
	// Print ')'
	PrintSymbol();

	// Print '{'
	PrintSymbol();
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::VAR)
		CompileVarDec();

	// Function definition with the number of local variables in the local scope.
	m_pCodeWriter->WriteFunction(functionName, m_SymbolTable.VarCount(SymbolTable::Kind::VAR));
	if (isMethod)
	{
		m_pCodeWriter->WritePush(VMWriter::Segment::ARG, 0);
		m_pCodeWriter->WritePop(VMWriter::Segment::POINTER, 0);
	}
	else if (isConstructor)
	{
		// Allocate memory for *this* object.
		m_pCodeWriter->WritePush(VMWriter::Segment::CONST, m_SymbolTable.VarCount(SymbolTable::Kind::FIELD));
		m_pCodeWriter->WriteCall("Memory.alloc", 1);
		m_pCodeWriter->WritePop(VMWriter::Segment::POINTER, 0);
	}

	// Statements in subroutine body.
	CompileStatements();
	// Print '}'
	PrintSymbol();
}

void JackCompilationEngine::CompileParameterList()
{
	// If ')', then have an empty list.
	if (m_pTokenizer->GetTokenType() != JackTokenizer::TokenType::SYMBOL ||
		m_pTokenizer->GetSymbol() != ')')
	{
		// Parameter type.
		PrintType();
		// Parameter name.
		PrintIdentifier();
		m_SymbolTable.Define(m_CurrentIdentifier.name, m_CurrentIdentifier.type, SymbolTable::Kind::ARG);

		// Possible list of comman-separated variable names in addition to the first.
		while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
			m_pTokenizer->GetSymbol() == ',')
		{
			// Print ','
			PrintSymbol();
			PrintType();
			PrintIdentifier();
			m_SymbolTable.Define(m_CurrentIdentifier.name, m_CurrentIdentifier.type, SymbolTable::Kind::ARG);
		}
	}
}

void JackCompilationEngine::CompileVarDec()
{
	PrintKeyWord("var", JackTokenizer::KeyWord::VAR);

	// Assign type of the local variable.
	PrintType();

	// Assign local variable name.
	PrintIdentifier();
	m_SymbolTable.Define(m_CurrentIdentifier.name, m_CurrentIdentifier.type, SymbolTable::Kind::VAR);
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == ',')
	{
		// Print ','
		PrintSymbol();
		PrintIdentifier();
		m_SymbolTable.Define(m_CurrentIdentifier.name, m_CurrentIdentifier.type, SymbolTable::Kind::VAR);
	}
	// Print ';'
	PrintSymbol();
}

void JackCompilationEngine::CompileStatements()
{
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
}

void JackCompilationEngine::CompileDo()
{
	PrintKeyWord("do", JackTokenizer::KeyWord::DO);

	// Compile the subroutine call and its expression list.
	CompileExpressionList();

	// Print ';'
	PrintSymbol();
	// void subroutine returns 0; ignore it (get it off the stack).
	m_pCodeWriter->WritePop(VMWriter::Segment::TEMP, 0);
}

void JackCompilationEngine::CompileLet()
{
	PrintKeyWord("let", JackTokenizer::KeyWord::LET);

	// Name of vairable being assigned.
	PrintIdentifier();

	// Memory segment that the current variable belongs to.
	VMWriter::Segment segment = GetVarSegment(m_CurrentIdentifier.name);

	// index of the variable within the segment.
	int varIndex = m_SymbolTable.IndexOf(m_CurrentIdentifier.name);

	// Handle array access.
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == '[')
	{
		// Push the base array pointer.
		m_pCodeWriter->WritePush(segment, varIndex);

		// Print '['
		PrintSymbol();

		// Compute the array index.
		CompileExpression();

		// Print ']'
		PrintSymbol();

		// Add the array index to the base of the pointer.
		m_pCodeWriter->WriteArithmetic(VMWriter::Command::ADD);
		
		// Access the array entry using the 'that' segment.
		m_pCodeWriter->WritePop(VMWriter::Segment::POINTER, 1);

		// Print '='
		PrintSymbol();

		// New value of variable is right-hand-side expression.
		CompileExpression();

		// Print ';'
		PrintSymbol();

		// Pop the computed expression into place.
		m_pCodeWriter->WritePop(VMWriter::Segment::THAT, 0);
	}
	// Handle non-array access.
	else
	{
		// Print '='
		PrintSymbol();

		// New value of variable is right-hand-side expression.
		CompileExpression();
		// Print ';'
		PrintSymbol();
		m_pCodeWriter->WritePop(segment, varIndex);
	}
}

void JackCompilationEngine::CompileWhile()
{
	PrintKeyWord("while", JackTokenizer::KeyWord::WHILE);

	std::string trueLabel = "WHILE_TRUE" + std::to_string(m_LabelCounter);
	std::string falseLabel = "WHILE_FALSE" + std::to_string(m_LabelCounter);

	// Increase label counter for the purpose of uniquely defined labels.
	m_LabelCounter++;

	// Start of while loop.
	m_pCodeWriter->WriteLabel(trueLabel);

	// Print '('
	PrintSymbol();

	// Boolean expression to test while loop.
	CompileExpression();

	// Print ')'
	PrintSymbol();

	// Negate the computed condition.
	m_pCodeWriter->WriteArithmetic(VMWriter::Command::NOT);

	// Jump to IF_FALSE if condition was not satisfied.
	m_pCodeWriter->WriteIf(falseLabel);

	// Print '{'
	PrintSymbol();

	// While loop body.
	CompileStatements();

	// End of body; repeat loop.
	m_pCodeWriter->WriteGoto(trueLabel);

	// Print '}'
	PrintSymbol();

	// Loop condition no longer met.
	m_pCodeWriter->WriteLabel(falseLabel);
}

void JackCompilationEngine::CompileReturn()
{
	PrintKeyWord("return", JackTokenizer::KeyWord::RETURN);

	// Compute the expression being returned.
	if (m_pTokenizer->GetTokenType() != JackTokenizer::TokenType::SYMBOL ||
		m_pTokenizer->GetSymbol() != ';')
		CompileExpression();
	// It's a void function, so it must return 0.
	else
		m_pCodeWriter->WritePush(VMWriter::Segment::CONST, 0);

	// Print ';'
	PrintSymbol();
	m_pCodeWriter->WriteReturn();
}

void JackCompilationEngine::CompileIf()
{
	std::string caseTrueLabel = "IF_TRUE" + std::to_string(m_LabelCounter);
	std::string caseFalseLabel = "IF_FALSE" + std::to_string(m_LabelCounter);

	// Increase label counter for the purpose of uniquely defined labels.
	m_LabelCounter++;

	PrintKeyWord("if", JackTokenizer::KeyWord::IF);

	// Print '('
	PrintSymbol();

	// Compute if statement condition.
	CompileExpression();

	// Print ')'
	PrintSymbol();

	// Negate the computed condition.
	m_pCodeWriter->WriteArithmetic(VMWriter::Command::NOT);

	// Jump to caseFalseLabel if condition is not satisfied.
	m_pCodeWriter->WriteIf(caseFalseLabel);

	// Print '{'
	PrintSymbol();

	// Body of the if statement.
	CompileStatements();

	// Print '}'
	PrintSymbol();

	// The 'if' condition was met, so skip the 'else' suit.
	m_pCodeWriter->WriteGoto(caseTrueLabel);

	// Condition wasn't met, so carry out the 'else' suit.
	m_pCodeWriter->WriteLabel(caseFalseLabel);
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::ELSE)
	{
		PrintKeyWord("else", JackTokenizer::KeyWord::ELSE);
		// Print '{'
		PrintSymbol();
		// Body of the else suit.
		CompileStatements();
		// Print '}'
		PrintSymbol();
	}

	m_pCodeWriter->WriteLabel(caseTrueLabel);
}

void JackCompilationEngine::CompileExpression()
{
	// Carry out a post-order expression evaluation.
	CompileTerm();
	while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL)
	{
		char sym = m_pTokenizer->GetSymbol();
		if (sym == '+' || sym == '-' || sym == '*' || sym == '/' || sym == '&' ||
			sym == '|' || sym == '<' || sym == '>' || sym == '=')
		{
			PrintSymbol();
			CompileTerm();
			if (sym == '+')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::ADD);
			else if (sym == '-')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::SUB);
			else if (sym == '*')
				m_pCodeWriter->WriteCall("Math.multiply", 2);
			else if (sym == '/')
				m_pCodeWriter->WriteCall("Math.divide", 2);
			else if (sym == '&')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::AND);
			else if (sym == '|')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::OR);
			else if (sym == '<')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::LT);
			else if (sym == '>')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::GT);
			else if (sym == '=')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::EQ);
		}
		else
			break;
	}
}

void JackCompilationEngine::CompileTerm()
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::STRING_CONST)
	{
		std::string newString = m_pTokenizer->GetStringVal();

		// Push in the String.new argument, i.e., the number of characters.
		m_pCodeWriter->WritePush(VMWriter::Segment::CONST, newString.size());

		// Construct a string object and store in temp segment.
		m_pCodeWriter->WriteCall("String.new", 1);
		m_pCodeWriter->WritePop(VMWriter::Segment::TEMP, 0);
		for (int i = 0; i < newString.size(); i++)
		{
			// Push in the string.
			m_pCodeWriter->WritePush(VMWriter::Segment::TEMP, 0);

			// Push in the ASCII number of the character, and append the character.
			m_pCodeWriter->WritePush(VMWriter::Segment::CONST, (int) newString[i]);
			m_pCodeWriter->WriteCall("String.appendChar", 2);

			// Store returned string in temp segment.
			m_pCodeWriter->WritePop(VMWriter::Segment::TEMP, 0);
		}
		// Push the resulting string onto stack for the caller.
		m_pCodeWriter->WritePush(VMWriter::Segment::TEMP, 0);

		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::INT_CONST)
	{
		m_pCodeWriter->WritePush(VMWriter::Segment::CONST, m_pTokenizer->GetIntVal());
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD)
	{
		if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::TRUE)
		{
			// NOT 0 gives -1, which is used to represent TRUE.
			m_pCodeWriter->WritePush(VMWriter::Segment::CONST, 1);
			m_pCodeWriter->WriteArithmetic(VMWriter::Command::NEG);
			PrintKeyWord("true", JackTokenizer::KeyWord::TRUE);
		}
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::FALSE)
		{
			// 0 represents false.
			m_pCodeWriter->WritePush(VMWriter::Segment::CONST, 0);
			PrintKeyWord("false", JackTokenizer::KeyWord::FALSE);
		}
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::JNULL)
		{
			// 0 represents null.
			m_pCodeWriter->WritePush(VMWriter::Segment::CONST, 0);
			PrintKeyWord("null", JackTokenizer::KeyWord::JNULL);
		}
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::THIS)
		{
			// Push in address of 'this'.
			m_pCodeWriter->WritePush(VMWriter::Segment::POINTER, 0);
			PrintKeyWord("this", JackTokenizer::KeyWord::THIS);
		}
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL)
	{
		char sym = m_pTokenizer->GetSymbol();
		// Parenthesized expression.
		if (sym == '(')
		{
			// Print '('
			PrintSymbol();
			CompileExpression();
			// Print ')'
			PrintSymbol();
		}
		// 'unaryOp(term)' expression.
		else if (sym == '-' || sym == '~')
		{
			// Print '-' or '~'
			PrintSymbol();
			CompileTerm();
			if (sym == '-')
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::NEG);
			else
				m_pCodeWriter->WriteArithmetic(VMWriter::Command::NOT);
		}
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		// Must check if variable, an array entry, or a subroutine call.
		m_CurrentIdentifier.name = m_pTokenizer->GetIdentifier();
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();

		char sym = m_pTokenizer->GetSymbol();
		// It's a variable name corresponding to an array.
		if (sym == '[')
		{
			VMWriter::Segment segment = GetVarSegment(m_CurrentIdentifier.name);
			// Save the old 'that' before the current array access.
			m_pCodeWriter->WritePush(VMWriter::Segment::POINTER, 1);
			m_pCodeWriter->WritePop(VMWriter::Segment::TEMP, 0);

			// Set the pointer to base address of the array.
			m_pCodeWriter->WritePush(segment, m_SymbolTable.IndexOf(m_CurrentIdentifier.name));

			// Print '['
			PrintSymbol();

			// Compute array index expression.  
			CompileExpression();

			// Print ']'
			PrintSymbol();

			// Calculate displacement from base pointer address.
			m_pCodeWriter->WriteArithmetic(VMWriter::Command::ADD);

			// Put address in THAT segment.
			m_pCodeWriter->WritePop(VMWriter::Segment::POINTER, 1);

			// Get value at THAT, i.e. the value of the array at the computed index.
			m_pCodeWriter->WritePush(VMWriter::Segment::THAT, 0);

			// Restore the original 'that' address
			m_pCodeWriter->WritePush(VMWriter::Segment::TEMP, 0);
			m_pCodeWriter->WritePop(VMWriter::Segment::POINTER, 1);
		}
		// It's a subroutine call.
		else if (sym == '.' || sym == '(')
			CompileExpressionList();
		// It's a regular variable access.
		else
		{
			VMWriter::Segment segment = GetVarSegment(m_CurrentIdentifier.name);
			m_pCodeWriter->WritePush(segment, m_SymbolTable.IndexOf(m_CurrentIdentifier.name));
		}
	}
}

void JackCompilationEngine::CompileExpressionList()
{
	std::string fullFuncName;
	int numArguments = 0;

	// Case 1: Subroutine called from a do statement.
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		fullFuncName = m_pTokenizer->GetIdentifier();
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	// Case 2: Subroutine called within an expression.
	else
		fullFuncName = m_CurrentIdentifier.name;

	// Class function or method of a non-primitive type variable.
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
		m_pTokenizer->GetSymbol() == '.')
	{
		// Method of a non-primitive type variable.
		if (m_SymbolTable.KindOf(fullFuncName) != SymbolTable::Kind::NONE)
		{
			// Segment of the variable.
			VMWriter::Segment segment = GetVarSegment(fullFuncName);

			// Push reference to the named object as an argument to the method call.
			numArguments++;
			m_pCodeWriter->WritePush(segment, m_SymbolTable.IndexOf(fullFuncName));

			// The function name is prepended by the variable's class type.
			fullFuncName = m_SymbolTable.TypeOf(fullFuncName);
		}
		// Print '.'
		PrintSymbol();
		
		// Function name has the format: (ClassName | varName).subroutineName.
		PrintIdentifier();
		fullFuncName = fullFuncName + "." + m_CurrentIdentifier.name;
	}
	// The current subroutine is a method on *this* object.
	else
	{
		// Prepend method name with the name of the current class.
		fullFuncName = m_ClassName + "." + fullFuncName;

		// Pass in address of *this* object as first argument.
		numArguments++;
		m_pCodeWriter->WritePush(VMWriter::Segment::POINTER, 0);
	}
	// Print '('
	PrintSymbol();

	// ')' indicates the end of the expression list.
	if (m_pTokenizer->GetTokenType() != JackTokenizer::TokenType::SYMBOL ||
		m_pTokenizer->GetSymbol() != ')')
	{
		// Compute the subroutine's first argument expression.
		numArguments++;
		CompileExpression();

		// Compute other argument expressions.
		while (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL &&
			m_pTokenizer->GetSymbol() == ',')
		{
			numArguments++;
			// Print ','
			PrintSymbol();
			CompileExpression();
		}
	}
	m_pCodeWriter->WriteCall(fullFuncName, numArguments);
	// Print ')'
	PrintSymbol();
}

void JackCompilationEngine::PrintSymbol()
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::SYMBOL)
	{
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
}

void JackCompilationEngine::PrintKeyWord(std::string keyWordString, JackTokenizer::KeyWord keyWordToken)
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::KEYWORD &&
		m_pTokenizer->GetKeyWord() == keyWordToken)
	{
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
}

void JackCompilationEngine::PrintIdentifier()
{
	if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		// Save the identifier.
		m_CurrentIdentifier.name = m_pTokenizer->GetIdentifier();

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
			m_CurrentIdentifier.type = "int";
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::CHAR)
			m_CurrentIdentifier.type = "char";
		else if (m_pTokenizer->GetKeyWord() == JackTokenizer::KeyWord::BOOLEAN)
			m_CurrentIdentifier.type = "boolean";
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
	else if (m_pTokenizer->GetTokenType() == JackTokenizer::TokenType::IDENTIFIER)
	{
		m_CurrentIdentifier.type = m_pTokenizer->GetIdentifier();
		if (m_pTokenizer->HasMoreTokens())
			m_pTokenizer->Advance();
	}
}

VMWriter::Segment JackCompilationEngine::GetVarSegment(std::string& varName)
{
	VMWriter::Segment segment;
	switch (m_SymbolTable.KindOf(varName))
	{
	case(SymbolTable::Kind::ARG):
		return VMWriter::Segment::ARG;
	case(SymbolTable::Kind::VAR):
		return VMWriter::Segment::LOCAL;
	case(SymbolTable::Kind::STATIC):
		return VMWriter::Segment::STATIC;
	case(SymbolTable::Kind::FIELD):
		return VMWriter::Segment::THIS;
	default:
		return VMWriter::Segment::NONE;
	}
}