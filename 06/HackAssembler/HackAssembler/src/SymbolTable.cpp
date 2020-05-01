#include "SymbolTable.h"
#include <string>

SymbolTable::SymbolTable()
{
	// Pointers to base address of virtual memory segments.
	m_SymbolTable["SP"] = 0;
	m_SymbolTable["LCL"] = 1;
	m_SymbolTable["ARG"] = 2;
	m_SymbolTable["THIS"] = 3;
	m_SymbolTable["THAT"] = 4;

	// General purpose register addresses.
	for (int i = 0; i < 16; i++)
		m_SymbolTable["R" + std::to_string(i)] = i;
	m_SymbolTable["SCREEN"] = 16384;
	m_SymbolTable["KBD"] = 24576;
}

void SymbolTable::AddEntry(std::string& symbol, int address)
{
	m_SymbolTable[symbol] = address;
}

bool SymbolTable::Contains(std::string& symbol) const
{
	return m_SymbolTable.find(symbol) != m_SymbolTable.end();
}

int SymbolTable::GetAddress(std::string& symbol) 
{
	return m_SymbolTable[symbol];
}