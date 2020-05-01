#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include <filesystem>
namespace fs = std::filesystem;


#include "VMParser.h"
#include "VMCodeWriter.h"

void saveFilePath(std::vector<std::string>& paths, const std::string& newPath);

int main(int argc, char* argv[])
{
	std::string path = argv[1];

	// Extensionless absolute paths.
	std::vector<std::string> absolutePaths;

	// Name for the single output assembly file.
	std::string programName;

	if (fs::is_regular_file(path))
	{
		std::string programName = path;
		saveFilePath(absolutePaths, path);
	}
	else if (fs::is_directory(path))
	{
		for (const auto& entry : fs::directory_iterator(path))
			saveFilePath(absolutePaths, entry.path().string());
	}

	//Create code writer to output to single .asm file.
	VMCodeWriter codeWriter(programName + ".asm");

	//Parse and write code for all ".vm" files in the current folder.
	for (std::string& vmFileName : absolutePaths)
	{
		VMParser parser(vmFileName + ".vm");
		codeWriter.SetFileName(vmFileName);
		while (parser.HasMoreCommands())
		{
			parser.Advance();
			VMParser::CommandType command = parser.GetCommandType();
			if (command == VMParser::CommandType::C_ARITHMETIC)
				codeWriter.WriteArithmetic(parser.Arg1());
			else if (command == VMParser::CommandType::C_PUSH ||
				command == VMParser::CommandType::C_POP)
				codeWriter.WritePushPop(command, parser.Arg1(), parser.Arg2());
		}
	}
	codeWriter.Close();

}

void saveFilePath(std::vector<std::string>& paths, const std::string& newPath)
{
	size_t extensionPos = newPath.find(".jack");
	if (extensionPos != -1)
	{
		std::string absolutePathNoExt = newPath.substr(0, extensionPos);
		paths.push_back(absolutePathNoExt);
	}
}