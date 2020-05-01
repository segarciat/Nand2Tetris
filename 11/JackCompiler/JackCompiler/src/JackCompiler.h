#pragma once

#include <iostream>
#include <vector>
#include "JackTokenizer.h"
#include "JackCompilationEngine.h""
#include "SymbolTable.h"
#include "VMWriter.h"

class JackCompiler
{
public:
	JackCompiler(std::vector<std::string>& sourceFileNames);
	void Compile();

private:
	std::vector<std::string> m_SourceFileNames;
};