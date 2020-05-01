#include "JackTokenizer.h"
#include <string>
#include <regex>

JackTokenizer::JackTokenizer(std::string filename):
	m_CurrentToken(""), m_CurrentTokenType(TokenType::NONE)
{
	m_pFile = new std::ifstream(filename);
	if (!m_pFile->is_open())
		std::cout << "Unable to open " << filename << std::endl;
}

JackTokenizer::~JackTokenizer()
{
	m_pFile->close();
	delete m_pFile;
}

bool JackTokenizer::HasMoreTokens()
{
	while (m_pFile->peek() != -1)
	{
		char c = m_pFile->peek();

		// Shave off the space.
		if (isspace(c))
			m_pFile->get();
		// It's a comment: get rid of the line.
		else if (c == '/')
		{
			// Advance the pointer to next char.
			m_pFile->get();

			// Possibly a multi-line comment.
			if (m_pFile->peek() == '*')
			{
				while (m_pFile->peek() != '/')
					m_pFile->ignore(std::numeric_limits<std::streamsize>::max(), '*');
				getline(*m_pFile, m_CurrentToken);
			}
			// It's actually a division symbol.
			else if (m_pFile->peek() != '/')
			{
				m_pFile->unget();
				return true;
			}
			// Regular, single line comment.
			else if (m_pFile->peek() == '/')
				getline(*m_pFile, m_CurrentToken);
		}
		// It's a token.
		else
			return true;
	}
	// No tokens, or all characters were spaces or comments.
	m_CurrentTokenType = TokenType::NONE;
	m_CurrentToken = "";
	return false;
}

void JackTokenizer::Advance()
{
	char c = m_pFile->get();
	// Clear old token string.
	m_CurrentToken = c;

	if (m_SYMBOLS.find(c) != m_SYMBOLS.end())
		m_CurrentTokenType = TokenType::SYMBOL;
	else if (isdigit(c))
	{
		while ( isdigit(m_pFile->peek()) )
			m_CurrentToken += m_pFile->get();
		m_CurrentTokenType = TokenType::INT_CONST;
	}
	else if (c == '"')
	{
		// Ignore leading '"' and clear the token string.
		m_CurrentToken = "";
		while (m_pFile->peek() != '"')
		{
			char newC = m_pFile->get();
			m_CurrentToken += newC;
		}
		// Remove right-delimiting '"'
		m_pFile->get();
		m_CurrentTokenType = TokenType::STRING_CONST;
	}
	// c is an underscore or alphanumerical.
	else if (c == '_' || isalnum(c))
	{
		while (m_pFile->peek() == '_' || isalnum(m_pFile->peek()))
			m_CurrentToken += m_pFile->get();

		if (m_KWMAP.find(m_CurrentToken) != m_KWMAP.end())
			m_CurrentTokenType = TokenType::KEYWORD;
		else
			m_CurrentTokenType = TokenType::IDENTIFIER;
	}
}

JackTokenizer::TokenType JackTokenizer::GetTokenType() const
{
	return m_CurrentTokenType;
}

std::string JackTokenizer::GetKeyWord()
{
	return m_CurrentToken;
}

char JackTokenizer::GetSymbol()
{
	return m_CurrentToken[0];
}

std::string JackTokenizer::GetIdentifier() const
{
	return m_CurrentToken;
}

int JackTokenizer::GetIntVal() const
{
	return stoi(m_CurrentToken);
}

std::string JackTokenizer::GetStringVal() const
{
	return m_CurrentToken;
}

void JackTokenizer::CleanLine()
{
	// Remove comments from end.
	size_t i = m_CurrentToken.find("//");
	if (i != -1)
		m_CurrentToken.erase(i, std::string::npos);

	// Removing leading and trailing spaces.
	m_CurrentToken = std::regex_replace(m_CurrentToken, std::regex("^(\\s+)|(\\s+)$"), "");
}