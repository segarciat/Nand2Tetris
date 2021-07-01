#include "JackTokenizer.h"
#include "JackTokenError.h"


const std::unordered_set<std::string> JackTokenizer::s_KWSet = {
	"class", "constructor", "function",  "method",  "field",
	"static", "var", "int", "char", "boolean", "void",
	"true", "false", "null", "this", "let", "do", "if",
	"else", "while", "return",
};

const std::unordered_set<char> JackTokenizer::s_SymSet = {
	'{', '}', '(', ')', '[', ']', '.',
	',', ';', '+', '-', '*', '/', '&',
	'|', '<', '>', '=', '~'
};

const int JackTokenizer::s_MAX_INT = 32767;

JackTokenizer::JackTokenizer(const std::filesystem::path& path)
{
	m_Ifs.open(path.string());
	if (!m_Ifs) {
		std::string msg = "Problem encountered while opening \"" + path.string() + "\"";
		throw std::ifstream::failure(msg.c_str());
	}
}

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
	if (s_SymSet.find(c) != s_SymSet.end())			// Symbol; already assigned above.
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

JackTokenType JackTokenizer::TokenType()
{
	if (s_SymSet.find(m_CurrentToken[0]) != s_SymSet.end())
		return JackTokenType::SYMBOL;
	else if (isdigit(m_CurrentToken[0]))
		return JackTokenType::INT_CONST;
	else if (m_CurrentToken[0] == '"')
		return JackTokenType::STRING_CONST;
	else if (s_KWSet.find(m_CurrentToken) != s_KWSet.end())
		return JackTokenType::KEYWORD;
	else if (isalnum(m_CurrentToken[0]) || m_CurrentToken[0] == '_')
		return JackTokenType::IDENTIFIER;
	else
		return JackTokenType::INVALID;
}

/* Should only be called if TokenType() == KEYWORD. */
const std::string JackTokenizer::KeyWord()
{
	return m_CurrentToken;
}

/* Should only be called if TokenType() == SYMBOL. */
char JackTokenizer::Symbol()
{
	return m_CurrentToken[0];
}

/* Should only be called if TokenType() == IDENTIFIER. */
const std::string JackTokenizer::Identifier()
{
	return m_CurrentToken;
}

/* Should only be called if TokenType() == INT_CONST. */
int JackTokenizer::IntVal()
{
	return std::stoi(m_CurrentToken);
}

/* Should only be called if TokenType() == STRING_CONST. */
const std::string JackTokenizer::StringVal()
{
	return m_CurrentToken.substr(1); // Omit the opening " (has no ending ").
}
