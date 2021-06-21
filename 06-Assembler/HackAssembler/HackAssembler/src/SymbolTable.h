#pragma once
#include <unordered_map>
#include <string>

/*
* Keeps correspondence between symbolic labls and numeric addresses
*/
class SymbolTable
{
private:
	std::unordered_map<std::string, int> m_ST;
public:
	SymbolTable();
	void AddEntry(const std::string& symbol, int address);
	bool Contains(const std::string& symbol) const;
	int GetAddress(const std::string& symbol) const;
};