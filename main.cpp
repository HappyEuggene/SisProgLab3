#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <map>
#include <set>
#include <Windows.h>

std::map<std::string, std::string> tokenClasses = {
        {"[0-9]+", "decimal number"},
        {"[0-9]*\\.[0-9]+", "floating point number"},
        {"0[xX][0-9a-fA-F]+", "hexadecimal number"},
        {"\"(\\\\.|[^\"])*\"", "string constant"},
        {"'(\\\\.|[^'])*'", "character constant"},
        {"//[^\n]*", "comment"},
        {"\\b(auto|break|case|char|const|continue|default|do|double|else|enum|extern|float|for|goto|if|int|long|register|return|short|signed|sizeof|static|struct|switch|typedef|union|unsigned|void|volatile|while)\\b", "reserved word"},
        {"\\+|\\-|\\*|\\/|\\%|\\=|\\<|\\>|\\&|\\|\\^|\\~|\\!|\\?", "operator"},
        {";|,|\\(|\\)|\\[|\\]|\\{|\\}|\\.|\\&|\\*|\\+|\\-|\\~|\\!|\\/|\\%|\\<|\\=|\\>|\\^|\\|\\?", "punctuation"},
        {"[a-zA-Z_][a-zA-Z0-9_]*", "identifier"}
};

std::set<std::string> recognizedTokens;

const size_t MAX_IDENTIFIER_LENGTH = 32;

void analyze(const std::string& code) {
    std::ofstream outFile("out.txt");
    HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);

    for (auto &pair : tokenClasses) {
        std::regex r(pair.first);
        std::sregex_iterator next(code.begin(), code.end(), r);
        std::sregex_iterator end;

        while (next != end) {
            std::smatch match = *next;
            std::string token = match.str();

            if (pair.second == "identifier" && token.length() > MAX_IDENTIFIER_LENGTH) {
                outFile << "<" << token << ", error>" << std::endl;
                SetConsoleTextAttribute(handle, FOREGROUND_RED);
                std::cout << token << " (ERROR) ";
            } else {
                recognizedTokens.insert(token);  // Add token to recognized set

                // Color output and save to file
                SetConsoleTextAttribute(handle, FOREGROUND_RED);
                if (pair.second == "comment") {
                    SetConsoleTextAttribute(handle, FOREGROUND_GREEN);
                } else if (pair.second == "reserved word") {
                    SetConsoleTextAttribute(handle, FOREGROUND_BLUE);
                }

                std::cout << token << " ";
                outFile << "<" << token << ", " << pair.second << ">" << std::endl;
            }

            next++;
        }
    }

    // Check for unrecognized tokens
    size_t lastPos = 0;
    for (const auto& token : recognizedTokens) {
        size_t pos = code.find(token, lastPos);
        if (pos != std::string::npos) {
            if (pos > lastPos) {
                std::string unrecognized = code.substr(lastPos, pos - lastPos);
                outFile << "<" << unrecognized << ", error>" << std::endl;
            }
            lastPos = pos + token.size();
        }
    }

    outFile.close();
}

int main() {
    std::ifstream inFile("input.txt");
    if (!inFile) {
        std::cerr << "Cannot open input file!" << std::endl;
        return 1;
    }

    std::string code((std::istreambuf_iterator<char>(inFile)),
                     std::istreambuf_iterator<char>());

    analyze(code);

    inFile.close();
    return 0;
}