#pragma once

#include <iostream>
#include <map>

/**
	Purpose: Provides a symbol table abstraction. The symbol table
	associates the identifier names found in the program with
	identifier properties needed for compilation: type, kind,
	and running index. The symbol table for Jack programs has
	two nested scopes (class/subroutine).
*/

class SymbolTable
{
public:
	enum class Kind
	{
		STATIC = 0, FIELD, ARG, VAR, NONE
	};

	struct Identifier
	{
		std::string name = "";
		std::string type = "";
		Kind kind = Kind::NONE;
		int index = -1;
	};

	// Intializes an empty symbol table.
	SymbolTable();

	// Starts a new subroutine scope (resets the subroutine's symbol table).
	void StartSubroutine();

	// Defines a new identifier, and assigns it a running index.
	void Define(std::string name, std::string type, Kind kind);

	// Returns the number of variables of a given kind in the current scope.
	int VarCount(Kind kind);

	// Returns the kind of the named identified of the current scope.
	Kind KindOf(std::string& name);

	// Returns the type of the named identified in the current scope.
	std::string TypeOf(std::string& name);

	// Returns the index assigned to the named identified.
	int IndexOf(std::string& name);

private:
	// Symbol table for the class scope.
	std::map<std::string, Identifier> m_ClassTable;

	// Symbol table for the subroutine scope.
	std::map<std::string, Identifier> m_SubTable;
};