#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <filesystem>
#include "CodeWriter.h"
#include "Parser.h"
#include "InvalidCommand.h"

namespace fs = std::filesystem;

const std::string g_SRC_EXT = ".vm";
const std::string g_TARGET_EXT = ".asm";

void Usage(const std::string& programName);

int main(int argc, char* argv[])
{
	if (argc == 1)
	{
		Usage(fs::path(argv[0]).stem().string());
		return EXIT_FAILURE;
	}
	std::vector<fs::path> abs_file_paths;
	fs::path prgm_path = fs::absolute(argv[1]);
	if (fs::is_directory(prgm_path))
	{	// All VM files in given directory
		for (auto& f : fs::directory_iterator{ prgm_path })
			if (f.is_regular_file() && f.path().extension() == g_SRC_EXT)
				abs_file_paths.emplace_back(f);
	}	// Single VM file path file input
	else if (fs::is_regular_file(prgm_path) && prgm_path.extension() == g_SRC_EXT)
		abs_file_paths.push_back(prgm_path);
	else {
		Usage(fs::path(argv[0]).stem().string());
		return EXIT_FAILURE;
	}
	std::string program_name; prgm_path.stem().string();
	if (program_name == "")		// "C:\ProgramDir\"
		program_name = prgm_path.parent_path().stem().string();
	else                        // "C:\ProgramDir" or "C:\ProgramDir\Program.vm"
		program_name = prgm_path.stem().string();
	int line_count;
	try {
		CodeWriter writer{ program_name };
		for (const fs::path& fp : abs_file_paths)
		{ 
			// Pass only name
			writer.SetFileName(fp.stem().string());
			// Pass absolute path
			Parser parser{ fp };
			line_count = 0;
			while (parser.HasMoreCommands())
			{
				line_count++;
				parser.Advance();
				HackVM::CType ctype = parser.CommandType();
				if (ctype == HackVM::CType::C_ARITHMETIC)
					writer.WriteArithmetic(parser.Arg1());
				else if (ctype == HackVM::CType::C_PUSH)
					writer.WritePushPop("push", parser.Arg1(), parser.Arg2());
				else if (ctype == HackVM::CType::C_POP)
					writer.WritePushPop("pop", parser.Arg1(), parser.Arg2());
				else
					throw HackVM::InvalidCommand(fp.stem().string());
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "(" << line_count << "): ";
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/*
* On invalid command-line arguments, gives user usage information
*/
void Usage(const std::string& programName)
{
	std::cerr << "Usage: " << programName << " [FILE|DIR]" << std::endl;
	std::cerr << "Description: Convert input Hack VM file to assembly." << std::endl;
	std::cerr << "             If directory, convert all VM files in it to a single assembly file.";
	std::cerr << std::endl;
}

