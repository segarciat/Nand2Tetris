#include <iostream>
#include <filesystem>
#include <vector>
#include <list>
#include <fstream>

namespace fs = std::filesystem;

#include "JackTokenizer.h"

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

        JackTokenizer tokenizer(absPathNoExt + ".jack");
        std::ofstream tokensXML(absPathNoExt + "T.xml");
        tokensXML << "<tokens>" << std::endl;
        while (tokenizer.HasMoreTokens())
        {
            tokenizer.Advance();
            JackTokenizer::TokenType tokenType = tokenizer.GetTokenType();
            if (tokenType == JackTokenizer::TokenType::KEYWORD)
                tokensXML << "<keyword> " << tokenizer.GetKeyWord() << " </keyword>" << std::endl;
            else if (tokenType == JackTokenizer::TokenType::SYMBOL)
            {
                char sym = tokenizer.GetSymbol();
                if (sym == '<')
                    tokensXML << "<symbol> " << "&lt;" << " </symbol>\n";
                else if (sym == '>')
                    tokensXML << "<symbol> " << "&gt;" << " </symbol>\n";
                else if (sym == '&')
                    tokensXML << "<symbol> " << "&amp;" << " </symbol>\n";
                else
                    tokensXML << "<symbol> " << sym << " </symbol>\n";
                
            }
            else if (tokenType == JackTokenizer::TokenType::IDENTIFIER)
                tokensXML << "<identifier> " << tokenizer.GetIdentifier() << " </identifier>\n";
            else if (tokenType == JackTokenizer::TokenType::INT_CONST)
                tokensXML << "<integerConstant> " << tokenizer.GetIntVal() << " </integerConstant>\n";
            else if (tokenType == JackTokenizer::TokenType::STRING_CONST)
                tokensXML << "<stringConstant> " << tokenizer.GetStringVal() << " </stringConstant>\n";
        }
        tokensXML << "</tokens>" << std::endl;
        tokensXML.close();
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