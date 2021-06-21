# pragma once
#include <unordered_map>
#include <string>

// Translate Hack assembly language mnemonics into binary codes
class Code
{
private:
	static const std::unordered_map<std::string, std::string> s_DestMap;
	static const std::unordered_map<std::string, std::string> s_CompMap;
	static const std::unordered_map<std::string, std::string> s_JumpMap;
public:
	static std::string Dest(const std::string& dest);
	static std::string Comp(std::string comp);
	static std::string Jump(const std::string& jump);
};