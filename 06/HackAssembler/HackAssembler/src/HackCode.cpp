#include "HackCode.h"
#include <iostream>

HackCode::HackCode()
{
	//Commands that operate with A, so a-bit is 0.
	m_CompCodeMap["0"] = "0101010";
	m_CompCodeMap["1"] = "0111111";
	m_CompCodeMap["-1"] = "0111010";
	m_CompCodeMap["D"] = "0001100";
	m_CompCodeMap["A"] = "0110000";
	m_CompCodeMap["!D"] = "0001101";
	m_CompCodeMap["!A"] = "0110001";
	m_CompCodeMap["-D"] = "0001111";
	m_CompCodeMap["-A"] = "0110011";
	m_CompCodeMap["D+1"] = "0011111";
	m_CompCodeMap["A+1"] = "0110111";
	m_CompCodeMap["D-1"] = "0001110";
	m_CompCodeMap["A-1"] = "0110010";
	m_CompCodeMap["D+A"] = "0000010";
	m_CompCodeMap["D-A"] = "0010011";
	m_CompCodeMap["A-D"] = "0000111";
	m_CompCodeMap["D&A"] = "0000000";
	m_CompCodeMap["D|A"] = "0010101";

	//Commands that operate with M, so a-bit is 1.
	m_CompCodeMap["M"] = "1110000";
	m_CompCodeMap["!M"] = "1110001";
	m_CompCodeMap["-M"] = "1110011";
	m_CompCodeMap["M+1"] = "1110111";
	m_CompCodeMap["M-1"] = "1110010";
	m_CompCodeMap["D+M"] = "1000010";
	m_CompCodeMap["D-M"] = "1010011";
	m_CompCodeMap["M-D"] = "1000111";
	m_CompCodeMap["D&M"] = "1000000";
	m_CompCodeMap["D|M"] = "1010101";
}

std::string HackCode::Dest(const std::string& destMnemonic) const
{
	// Destination control bits.
	char destCode[] = "111";
	// Do not update the register that A refers to.
	if (destMnemonic.find('M') == -1)
		destCode[2] = '0';
	// Do not update the data register D.
	if (destMnemonic.find('D') == -1)
		destCode[1] = '0';
	// Do not update the A register.
	if (destMnemonic.find('A') == -1)
		destCode[0] = '0';

	return std::string(destCode);
}

std::string HackCode::Comp(const std::string& compMnemonic)
{
	return m_CompCodeMap[compMnemonic];
}

std::string HackCode::Jump(const std::string& jumpMnemonic) const
{
	if (jumpMnemonic == "JMP")
		return "111";
	if (jumpMnemonic == "JNE")
		return "101";

	// Initialize to a no-jump.
	char jumpCode[] = "000";

	// Jump on greater than zero.
	if (jumpMnemonic.find('G') != -1)
		jumpCode[2] = '1';

	// Jump on equal to zero.
	if (jumpMnemonic.find('E') != -1)
		jumpCode[1] = '1';

	// Jump on less than zero.
	if (jumpMnemonic.find('L') != -1)
		jumpCode[0] = '1';
	
	return std::string(jumpCode);
}