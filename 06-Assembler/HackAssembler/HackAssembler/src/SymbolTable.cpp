#include "SymbolTable.h"

// Initializes symbol table with predefined symbols
SymbolTable::SymbolTable()
	:m_ST{ 
		{"SP", 0}, 
		{"LCL", 1}, 
		{"ARG", 2}, 
		{"THIS", 3}, 
		{"THAT", 4}, 
		{"SCREEN", 16384}, 
		{"KBD", 24576} 
	}
{
	// General purpose register addresses
	for (int i = 0; i < 16; i++)
		m_ST.insert({ "R" + std::to_string(i), i });
}

void SymbolTable::AddEntry(const std::string& symbol, int address)
{
	m_ST.insert({ symbol, address });
}

bool SymbolTable::Contains(const std::string& symbol) const
{
	return m_ST.find(symbol) != m_ST.end();
}

/*
* Returns address associated with a symbol label
* Should only be called if Contains() was true
* 
* Allowing it to throw nullptr exception if user fails to rely on Contains()
*/
int SymbolTable::GetAddress(const std::string& symbol) const
{
	auto it = m_ST.find(symbol);
	return it->second;
}