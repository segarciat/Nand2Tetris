#include <iostream>
#include <string>
#include <vector>
#include <exception>
#include <filesystem>
#include <fstream>
#include "CompilationEngine.h"

namespace fs = std::filesystem;

const char* const g_SRC_EXT = ".jack";
const char* const g_TARGET_EXT = ".vm";

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
	{	// All Jack files in given directory.
		for (auto& f : fs::directory_iterator{ prgm_path })
			if (f.is_regular_file() && f.path().extension() == g_SRC_EXT)
				abs_file_paths.emplace_back(f);
	}	// Single Jack file path file input.
	else if (fs::is_regular_file(prgm_path) && prgm_path.extension() == g_SRC_EXT)
		abs_file_paths.push_back(prgm_path);
	else {
		Usage(fs::path(argv[0]).stem().string());
		return EXIT_FAILURE;
	}
	try 
	{
		for (const fs::path& p : abs_file_paths)
		{
			std::cout << "Processing " << p.string() << std::endl;
			const std::string& class_name = p.stem().string();
			// files closed at end of scope.
			std::ifstream ifs{ p.string() };
			std::ofstream ofs{ class_name + g_TARGET_EXT };
			jack::CompilationEngine engine{ ifs, ofs, class_name};
			engine.CompileClass();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}

/*
* On invalid command-line arguments, gives user usage information.
*/
void Usage(const std::string& programName)
{
	std::cerr << "Usage: " << programName << " [FILE|DIR]" << std::endl;
	std::cerr << "Description: Convert input Jack file to XML." << std::endl;
	std::cerr << "             If directory, convert each Jack file in it to a separate XML file.";
	std::cerr << std::endl;
}
