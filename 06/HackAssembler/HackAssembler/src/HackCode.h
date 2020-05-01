#pragma once

#include <iostream>
#include <map>

/**
	Purpose: Translates Hack assembly language mnemonics into binary codes.
*/

class HackCode
{
public:
	//Loads compute command codes onto map.
	HackCode();

	//Returns 3 destination control bits as a string.
	std::string Dest(const std::string& destMnemonic) const;

	//Returns 7 computational control bits as a string.
	std::string Comp(const std::string& compMnemonic);

	//Returns 3 jump control bits as a string.
	std::string Jump(const std::string& jumpMnemonic) const;

private:
	// Maps each Hack assembly expression to its corresponding sequence of control bits.
	std::map<std::string, std::string> m_CompCodeMap;
};