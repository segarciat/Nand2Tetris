#include "SymbolTable.h"

SymbolTable::SymbolTable()
{}

void SymbolTable::StartSubroutine()
{
	m_SubTable.clear();
}

void SymbolTable::Define(std::string name, std::string type, Kind kind)
{
	if (kind == Kind::VAR || kind == Kind::ARG)
		m_SubTable[name] = Identifier{ name, type, kind, VarCount(kind) };
	else if (kind == Kind::STATIC || kind == Kind::FIELD)
		m_ClassTable[name] = Identifier{ name, type, kind, VarCount(kind) };
	
}

int SymbolTable::VarCount(Kind kind)
{
	int count = 0;
	// Search the subroutine scope first, and then the outer class scope.
	if (kind == Kind::VAR || kind == Kind::ARG)
	{
		for (std::pair<const std::string, SymbolTable::Identifier>& aPair : m_SubTable)
			if (aPair.second.kind == kind)
				count++;
	}
	else if (kind == Kind::STATIC || kind == Kind::FIELD)
	{
		for (std::pair<const std::string, SymbolTable::Identifier>& aPair : m_ClassTable)
			if (aPair.second.kind == kind)
				count++;
	}
	return count;
}

SymbolTable::Kind SymbolTable::KindOf(std::string& name)
{
	// Search the subroutine scope first, and then the outer class scope.
	if (m_SubTable.find(name) != m_SubTable.end())
		return m_SubTable[name].kind;
	else if (m_ClassTable.find(name) != m_ClassTable.end())
		return m_ClassTable[name].kind;
	return Kind::NONE;
}

std::string SymbolTable::TypeOf(std::string& name)
{
	// Search the subroutine scope first, and then the outer class scope.
	if (m_SubTable.find(name) != m_SubTable.end())
		return m_SubTable[name].type;
	else if (m_ClassTable.find(name) != m_ClassTable.end())
		return m_ClassTable[name].type;
	// Indicates that no such no exists.
	return "";
}
int SymbolTable::IndexOf(std::string& name)
{
	// Search the subroutine scope first, and then the outer class scope.
	if (m_SubTable.find(name) != m_SubTable.end())
		return m_SubTable[name].index;
	else if (m_ClassTable.find(name) != m_ClassTable.end())
		return m_ClassTable[name].index;
	// Indicates that no such name exists.
	return -1;
}