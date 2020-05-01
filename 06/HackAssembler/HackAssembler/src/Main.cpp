#include <iostream>
#include <vector>
#include <filesystem>

// Requires at least C++17.
namespace fs = std::filesystem;

#include "HackWriter.h"

void SaveFilePath(std::vector<std::string>& paths, std::string& newPath);

int main(int argc, char* argv[]) {
	// path to directory with ".asm" files, or to a single ".asm" file.
	std::string path = argv[1];

	// Extensionless absolute paths (meaning, shave off ".asm").
	std::vector<std::string> absolutePaths;

	if (fs::is_regular_file(path))
		SaveFilePath(absolutePaths, path);
	else if (fs::is_directory(path))
		for (const auto& entry : fs::directory_iterator(path))
		{
			std::string absolutePath = entry.path().string();
			SaveFilePath(absolutePaths, absolutePath);
		}

	// Compile all the files.
	for (std::string& sourcePathNoExt : absolutePaths)
	{
		HackWriter hackWriter(sourcePathNoExt);
		hackWriter.WriteHackCode();
	}
}


void SaveFilePath(std::vector<std::string>& paths, std::string& newPath)
{
	std::string absolutePath = newPath;
	size_t extensionPos = absolutePath.find(".asm");
	if (extensionPos != -1)
	{
		std::string absolutePathNoExt = absolutePath.substr(0, extensionPos);
		paths.push_back(absolutePathNoExt);
	}
}