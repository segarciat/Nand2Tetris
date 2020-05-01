#include <iostream>
#include <filesystem>
#include <vector>
#include <fstream>

namespace fs = std::filesystem;

#include "JackCompilationEngine.h"

void saveFilePath(std::vector<std::string>& paths, std::string& newPath);

int main(int argc, char* argv[])
{
    std::string path = argv[1];

    // Extensionless absolute paths.
    std::vector<std::string> absolutePaths;

    if (fs::is_regular_file(path))
        saveFilePath(absolutePaths, path);
    else if (fs::is_directory(path))
        for (const auto& entry : fs::directory_iterator(path))
        {
            std::string absolutePath = entry.path().string();
            saveFilePath(absolutePaths, absolutePath);
        }

    for (std::string absPathNoExt : absolutePaths)
    {
        std::cout << ">> " << absPathNoExt << " <<\n";
        JackCompilationEngine compilationEngine(absPathNoExt + ".jack", absPathNoExt + ".xml");
        compilationEngine.CompileClass();
    }
}

void saveFilePath(std::vector<std::string>& paths, std::string& newPath)
{
    std::string absolutePath = newPath;
    size_t extensionPos = absolutePath.find(".jack");
    if (extensionPos != -1)
    {
        std::string absolutePathNoExt = absolutePath.substr(0, extensionPos);
        paths.push_back(absolutePathNoExt);
    }
}