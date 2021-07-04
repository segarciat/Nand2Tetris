#include <algorithm>		// std::find
#include <iterator>			// std::cbegin, std::cend
#include "JackTokenizer.h"
#include "JackTokenError.h"
#include "JackConstants.h"

namespace jack {
const char* const JackTokenizer::s_KWs[] = {
	jack::CLASS, jack::CONSTRUCTOR, jack::FUNCTION, jack::METHOD,
	jack::FIELD, jack::STATIC, jack::VAR, jack::INT, jack::CHAR,
	jack::BOOL, jack::VOID, jack::TRUE, jack::FALSE, jack::J_NULL,
	jack::THIS, jack::LET, jack::DO, jack::IF, jack::ELSE, jack::WHILE,
	jack::RETURN
};

const char JackTokenizer::s_Syms[] = {
	'{', '}', '(', ')', '[', ']', '.',
	',', ';', '+', '-', '*', '/', '&',
	'|', '<', '>', '=', '~'
};

const int JackTokenizer::s_MAX_INT = 32767;

JackTokenizer::JackTokenizer(std::ifstream& ifs)
	:m_Ifs{ ifs } {}

JackTokenizer::~JackTokenizer()
{
	if (m_Ifs.is_open())
		m_Ifs.close();
}

bool JackTokenizer::HasMoreTokens()
{
	char c;
	while ((c = m_Ifs.peek()) != EOF)
	{
		if (isspace(c))
			m_Ifs.get();
		else if ((c = m_Ifs.get()) == '/' && m_Ifs.peek() == '/')	// End-of-line comment.
			m_Ifs.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
		else if (c == '/' && m_Ifs.peek() == '*')					// Multiline/Doc comment.
		{
			m_Ifs.get();			// Consume the '*'.
			char c2 = m_Ifs.get();	// Multiline comment ends with */.
			while (((c = m_Ifs.get()) != '/' && c != EOF) || c2 != '*')	// Order of || matters.
				c2 = c;
			if (c == EOF)
				throw JackTokenError("Expected '*/'");
		}
		else {
			m_Ifs.putback(c);		// Character from end-of-line comment branch.
			return true;
		}
	}
	return false;
}

void JackTokenizer::Advance()
{
	char c = m_Ifs.get();		// HasMoreTokens() guarantees c is not EOF.
	m_CurrentToken = c;
	if (std::find(std::cbegin(s_Syms), std::cend(s_Syms), c) != std::cend(s_Syms))
		return;
	else if (isdigit(c))							// Possibly an integer constant
	{
		while (isdigit(m_Ifs.peek()))
			m_CurrentToken += m_Ifs.get();
		if (std::stoi(m_CurrentToken) > s_MAX_INT)
			throw new JackTokenError("Integer " + m_CurrentToken + " too large");
	}
	else if (c == '"')								// Possibly a string constant.
	{
		char c2 = c;
		while ((c2 == '\\' || (c = m_Ifs.get()) != '"') && c != EOF) {
			m_CurrentToken += c;					// Allow character escape sequences,
			c2 = c;									// including \".
		}
		if (c2 == EOF)								// Discard closing ".
			throw JackTokenError("Expected \"");
	}
	else if (isalnum(c) || c == '_')				// Identifier (variableName, className..).
		while ((isalnum(c = m_Ifs.peek()) || c == '_') && c != EOF)
			m_CurrentToken += (c = m_Ifs.get());
	else
		throw JackTokenError("Unexpected " + c);
}

JackTokenType JackTokenizer::TokenType() const
{
	char c = m_CurrentToken[0];
	if (std::find(std::cbegin(s_Syms), std::cend(s_Syms), c) != std::cend(s_Syms))
		return JackTokenType::SYMBOL;
	else if (isdigit(c))
		return JackTokenType::INT_CONST;
	else if (c == '"')
		return JackTokenType::STRING_CONST;
	else if (std::find(std::cbegin(s_KWs), std::cend(s_KWs), m_CurrentToken) != std::cend(s_KWs))
		return JackTokenType::KEYWORD;
	else if (isalnum(c) || c == '_')
		return JackTokenType::IDENTIFIER;
	else
		return JackTokenType::INVALID;
}

/* Should only be called if TokenType() == KEYWORD. */
const std::string JackTokenizer::KeyWord() const
{
	return m_CurrentToken;
}

/* Should only be called if TokenType() == SYMBOL. */
char JackTokenizer::Symbol() const
{
	return m_CurrentToken[0];
}

/* Should only be called if TokenType() == IDENTIFIER. */
const std::string JackTokenizer::Identifier() const
{
	return m_CurrentToken;
}

/* Should only be called if TokenType() == INT_CONST. */
int JackTokenizer::IntVal() const
{
	return std::stoi(m_CurrentToken);
}

/* Should only be called if TokenType() == STRING_CONST. */
const std::string JackTokenizer::StringVal() const
{
	return m_CurrentToken.substr(1); // Omit the opening " (has no ending ").
}
} // namespace jack