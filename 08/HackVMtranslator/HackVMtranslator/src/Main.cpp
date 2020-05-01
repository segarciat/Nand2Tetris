#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>

#include "VMParser.h"
#include "VMCodeWriter.h"
namespace fs = std::filesystem;

void saveFilePath(std::vector<std::string>& paths, const std::string& newPath);

int main(int argc, char* argv[])
{
	std::string path = argv[1];

	// Extensionless absolute paths.
	std::vector<std::string> absolutePaths;

	// Name for the single output assembly file.
	std::string programName = path.substr(0, path.find(".vm"));

	if (fs::is_regular_file(path))
		saveFilePath(absolutePaths, path);
	else if (fs::is_directory(path))
	{
		size_t fileNamePos = path.find_last_of('\\');
		programName = programName + path.substr(fileNamePos, path.size() - fileNamePos);
		std::cout << programName << std::endl;
		for (const auto& entry : fs::directory_iterator(path))
			saveFilePath(absolutePaths, entry.path().string());
	}

	//Create code writer to output to single .asm file.
	VMCodeWriter codeWriter(programName + ".asm");
	//Parse and write code for all ".vm" files in the current folder.
	for (std::string& vmFileName : absolutePaths)
	{
		std::cout << vmFileName << std::endl;
		//Move two directories up.
		VMParser parser(vmFileName + ".vm");
		codeWriter.SetFileName(vmFileName);
		while (parser.HasMoreCommands())
		{
			parser.Advance();
			VMParser::CommandType command = parser.GetCommandType();
			if (command == VMParser::CommandType::C_CALL)
				std::cout << "Call command.." << std::endl;
			if (command == VMParser::CommandType::C_ARITHMETIC)
				codeWriter.WriteArithmetic(parser.Arg1());
			else if (command == VMParser::CommandType::C_PUSH ||
				command == VMParser::CommandType::C_POP)
				codeWriter.WritePushPop(command, parser.Arg1(), parser.Arg2());
			else if (command == VMParser::CommandType::C_LABEL)
				codeWriter.WriteLabel(parser.Arg1());
			else if (command == VMParser::CommandType::C_IF)
				codeWriter.WriteIf(parser.Arg1());
			else if (command == VMParser::CommandType::C_GOTO)
				codeWriter.WriteGoto(parser.Arg1());
			else if (command == VMParser::CommandType::C_CALL)
				codeWriter.WriteCall(parser.Arg1(), parser.Arg2());
			else if (command == VMParser::CommandType::C_FUNCTION)
				codeWriter.WriteFunction(parser.Arg1(), parser.Arg2());
			else if (command == VMParser::CommandType::C_RETURN)
				codeWriter.WriteReturn();
		}
	}
	codeWriter.Close();
}

void saveFilePath(std::vector<std::string>& paths, const std::string& newPath)
{
	size_t extensionPos = newPath.find(".vm");
	if (extensionPos != -1)
	{
		std::string absolutePathNoExt = newPath.substr(0, extensionPos);
		paths.push_back(absolutePathNoExt);
	}
}