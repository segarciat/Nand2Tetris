#include "Code.h"
#include <algorithm>

const std::unordered_map<std::string, std::string> Code::s_DestMap = {
	{"", "000"},
	{"M", "001"},
	{"D", "010"},
	{"MD", "011"},
	{"A", "100"},
	{"AM", "101"},
	{"AD", "110"},
	{"AMD", "111"}
};

const std::unordered_map<std::string, std::string> Code::s_JumpMap = {
	{"", "000"},
	{"JGT", "001"},
	{"JEQ", "010"},
	{"JGE", "011"},
	{"JLT", "100"},
	{"JNE", "101"},
	{"JLE", "110"},
	{"JMP", "111"}
};

const std::unordered_map<std::string, std::string> Code::s_CompMap = {
	{"0", "101010"},
	{"1", "111111"},
	{"-1", "111010"},
	{"D", "001100"},
	{"A", "110000"},
	{"!D", "001101"},
	{"!A", "110001"},
	{"-D", "001111"},
	{"-A", "110011"},
	{"D+1", "011111"},
	{"A+1", "110111"},
	{"D-1", "001110"},
	{"A-1", "110010"},
	{"D+A", "000010"},
	{"D-A", "010011"},
	{"A-D", "000111"},
	{"D&A", "000000"},
	{"D|A", "010101"}
};

// Return the 3-bit binary code for dest, or "" if no match.
std::string Code::Dest(const std::string& dest)
{
	auto it = s_DestMap.find(dest);
	return (it == s_DestMap.end()) ? "" : it->second;
}

/*
* Return the 7-bit binary code for comp, or "" if no match.
* 
* aBit is 1 if comp refers to 'M', and 0 otherwise.
*/
std::string Code::Comp(std::string comp)
{
	int aBit = (comp.find('M') != std::string::npos);
	std::replace(comp.begin(), comp.end(), 'M', 'A');
	auto it = s_CompMap.find(comp);
	return (it == s_CompMap.end()) ? "" : std::to_string(aBit) + it->second;
}

// Return the 3-bit binary jump for dest, or "" if no match.
std::string Code::Jump(const std::string& jump)
{
	auto it = s_JumpMap.find(jump);
	return (it == s_JumpMap.end()) ? "" : it->second;
}