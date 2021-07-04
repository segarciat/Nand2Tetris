#pragma once
#include <string>
#include <fstream>
#include <unordered_set>

namespace jack {
/* Types of Lexical Elements of the Jack programming language. */
enum class JackTokenType
{
	KEYWORD, SYMBOL, IDENTIFIER, INT_CONST, STRING_CONST, INVALID
};

/*
* The JackTokenizer class parses an input jack source file
* (with a ".jack" extension) into its terminal elements (tokens).
* It also ignores white spaces and any valid type of comment.
*/
class JackTokenizer
{
private:
	// Valid Jack Keywords.
	static const char* const s_KWs[];
	// Valid Jack symbols.
	static const char s_Syms[];
	// Maximum integer constant in Jack.
	static const int s_MAX_INT;

	// Input filestream for the relevant Jack source file.
	std::ifstream& m_Ifs;
	// Current Jack token parsed from stream.
	std::string m_CurrentToken;
public:
	// Gets file stream ready for parsing.
	JackTokenizer(std::ifstream& ifs);
	~JackTokenizer();
	// Asserts that there is a token to be read (non-comment/white space).
	bool HasMoreTokens();
	// Sets the current token in the stream (should call only if HasMoreTokens() is true).
	void Advance();
	JackTokenType TokenType() const;

	// Returns value of current token (after asserting its type with TokenType()).
	const std::string KeyWord() const;
	char Symbol() const;
	const std::string Identifier() const;
	int IntVal() const;
	const std::string StringVal() const;
};
} // namespace jack