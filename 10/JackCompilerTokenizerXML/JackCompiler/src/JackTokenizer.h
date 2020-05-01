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
	~JackTokenizer();

	// Member functions.
	bool HasMoreTokens();
	void Advance();
	TokenType GetTokenType() const;
	std::string GetKeyWord();
	char GetSymbol();
	std::string GetIdentifier() const;
	int GetIntVal() const;
	std::string GetStringVal() const;

private:
	// Private member variables.
	std::ifstream* m_pFile;
	std::string m_CurrentToken;
	TokenType m_CurrentTokenType;
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
	const std::set<char> m_SYMBOLS = { '{', '}', '(', ')', '[', ']', '.',
								',', ';', '+', '-', '*', '/', '&',
								'|', '<', '>', '=', '~'};

	// Private methods.
	void CleanLine();
};