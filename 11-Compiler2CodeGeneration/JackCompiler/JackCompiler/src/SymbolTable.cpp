#include <algorithm>		// std::count_if
#include "SymbolTable.h"
#include "JackIdentifierError.h"

namespace jack {
void SymbolTable::StartSubroutine()
{
	m_FuncST.clear();
}

void SymbolTable::Define(const std::string& name, const std::string& type, Kind kind)
{
	if (kind == Kind::NONE)
		throw JackIdentifierError(name);				// Throw invalid kind.
	std::unordered_map<std::string, STEntry>& st =
		(kind == Kind::ARG || kind == Kind::VAR) ? m_FuncST : m_ClassST;
	// Verify identifier does not already exist.
	if (st.find(name) == st.end())
		st.insert({ name, {kind, type, VarCount(kind)} });
}

int SymbolTable::VarCount(Kind kind) const
{
	if (kind == Kind::NONE)
		throw JackIdentifierError("Invalid Identifier");// Throw invalid kind.
	const std::unordered_map<std::string, STEntry>& st =
		(kind == Kind::ARG || kind == Kind::VAR) ? m_FuncST : m_ClassST;
	// Lambda function in third argument compares by 'kind'.
	return std::count_if(st.begin(), st.end(),
		[&kind](const std::pair<std::string, STEntry>& p) {return p.second.kind == kind; });
}

SymbolTable::Kind SymbolTable::KindOf(const std::string& name) const
{
	std::unordered_map<std::string, STEntry>::const_iterator it;
	if ((it = m_FuncST.find(name)) == m_FuncST.end())
		if ((it = m_ClassST.find(name)) == m_ClassST.end())
			return Kind::NONE;
	return it->second.kind;
}

const std::string SymbolTable::TypeOf(const std::string& name) const
{
	std::unordered_map<std::string, STEntry>::const_iterator it;
	if ((it = m_FuncST.find(name)) == m_FuncST.end())
		if ((it = m_ClassST.find(name)) == m_ClassST.end())
			return "";
	return it->second.type;
}

int SymbolTable::IndexOf(const std::string& name) const
{
	std::unordered_map<std::string, STEntry>::const_iterator it;
	if ((it = m_FuncST.find(name)) == m_FuncST.end())
		if ((it = m_ClassST.find(name)) == m_ClassST.end())
			return -1;
	return it->second.index;
}
} // namespace jack