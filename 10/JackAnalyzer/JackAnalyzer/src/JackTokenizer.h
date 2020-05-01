#pragma once

#include <iostream>
#include <fstream>
#include <set>
#include <map>

class JackTokenizer
{
public:
	enum class TokenType
	{
		KEYWORD = 0,
		SYMBOL,
		IDENTIFIER,
		INT_CONST,
		STRING_CONST,
		NONE
	};

	enum class KeyWord
	{
		CLASS = 0, METHOD, FUNCTION, CONSTRUCTOR,
		INT, BOOLEAN, CHAR, VOID,
		VAR, STATIC, FIELD,
		LET, DO, IF, ELSE, WHILE, RETURN,
		TRUE, FALSE, JNULL, THIS
	};

	// Constructor.
	JackTokenizer(std::string filename);

	// Destructor to free file object memory.
	~JackTokenizer();

	// Member functions.
	bool HasMoreTokens();

	// Moves file pointer and stores next token and its type.
	void Advance();

	TokenType GetTokenType() const;

	// Called if the TokenType is KEYWORD.
	KeyWord GetKeyWord();

	// Called if the TokenType is SYMBOL.
	char GetSymbol();

	// Called if the TokenType is IDENTIFIER.
	std::string GetIdentifier() const;

	// Called if the TokenType is INT_CONST.
	int GetIntVal() const;

	// Called if the TokenType is STRING_CONST.
	std::string GetStringVal() const;

private:
	// Input Jack source file object.
	std::ifstream* m_pFile;

	// Current language token.
	std::string m_CurrentToken;

	// TokenType of the current language token.
	TokenType m_CurrentTokenType;

	// string-to-KeyWord map.
	std::map<std::string, KeyWord> m_KWMAP = {
		{"class", KeyWord::CLASS},
		{"constructor", KeyWord::CONSTRUCTOR },
		{"function", KeyWord::FUNCTION},
		{"method", KeyWord::METHOD},
		{"field", KeyWord::FIELD},
		{"static", KeyWord::STATIC},
		{"var", KeyWord::VAR},
		{"int", KeyWord::INT},
		{"char", KeyWord::CHAR},
		{"boolean", KeyWord::BOOLEAN},
		{"void", KeyWord::VOID},
		{"true", KeyWord::TRUE},
		{"false", KeyWord::FALSE},
		{"null", KeyWord::JNULL},
		{"this", KeyWord::THIS},
		{"let", KeyWord::LET},
		{"do", KeyWord::DO},
		{"if", KeyWord::IF},
		{"else", KeyWord::ELSE},
		{"while", KeyWord::WHILE},
		{"return", KeyWord::RETURN}
	};

	// Set of characters that a TokenType SYMBOL can be.
	const std::set<char> m_SYMBOLS = { '{', '}', '(', ')', '[', ']', '.',
								',', ';', '+', '-', '*', '/', '&',
								'|', '<', '>', '=', '~' };

	// Private methods.
	void CleanLine();
};