#pragma once

#include <map>
#include <string>

/*
	Purpose: Keeps a correspondence between symbolic labels and numeric addresses
	written for a Hack system.
*/

class SymbolTable
{
public:
	// Initialize hash table (map) to base pointers of virtual memory segments.
	SymbolTable();

	// Creates a new label/identifier with an associated RAM address.
	void AddEntry(std::string& symbol, int address);

	// Verifies whether a has already been defined.
	bool Contains(std::string& symbol) const;

	// Find the RAM address assigne to symbol.
	int GetAddress(std::string& symbol);

private:
	std::map<std::string, int> m_SymbolTable;
};