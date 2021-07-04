#pragma once
#include <string>
#include <unordered_map>

namespace jack {

/*
* The symbol table associates the identifier names found in the
* program with identifier properties needed for compilation:
* - Type
* - Kind
* - Running Index
* The symbol table for Jack programs has two nested scopes (class/subroutine)
*/

class SymbolTable
{
public:
	// Refers to the scope of a variable.
	enum class Kind {
		STATIC, FIELD, ARG, VAR, NONE
	};
public:
	// Clears the symbol table.
	void StartSubroutine();
	void Define(const std::string& name, const std::string& type, Kind kind);
	// Number of variables in symbol table of given kind.
	int VarCount(Kind kind) const;
	// Returns kind (scope) of identifier 'name' in symbol table, or NONE.
	Kind KindOf(const std::string& name) const;
	// Returns the type of identifier 'name' in symbol table.
	const std::string TypeOf(const std::string& name) const;
	// Returns the index of identifier 'name' in symbol table.
	int IndexOf(const std::string& name) const;
private:
	struct STEntry {
		Kind kind;
		const std::string type;
		int index;
	};
	// STATIC identifiers and class FIELDs.
	std::unordered_map<std::string, STEntry> m_ClassST;
	// Local (VAR) variables and ARGs.
	std::unordered_map<std::string, STEntry> m_FuncST;
};
} // namespace jack
