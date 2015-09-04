/*
Project: TUC
File: tuc.cpp
Author: Leonardo Banderali
Created: August 7, 2015
Last Modified: September 3, 2015

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2015 Leonardo Banderali

License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

// c++ standard libraries
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

// project headers
#include "lexer.hpp"

int main(int argc, char** argv) {
    if (argc == 3) {
        // read input file
        auto inputFile = std::ifstream{argv[1]};
        std::stringbuf sb;
        inputFile.get(sb, static_cast<char>(-1)); // read the entire file
        inputFile.close();
        const auto fileText = sb.str();

        // analyze the text
        auto tree = std::vector<std::vector<tuc::Token>>{}; // a very basic Abstract Syntax Tree (AST)
        auto lexer = tuc::make_lexer(fileText.cbegin(), fileText.cend());
        auto tokenBuffer = std::vector<tuc::Token>{};       // a buffer to store un processed tokens
        auto token = lexer.current();

        while(!token.empty()) {
            if (token.type() == "ADD") {
                auto prevToken = tokenBuffer.back();
                tokenBuffer.pop_back();
                auto nextToken = lexer.next();
                auto lastToken = lexer.next();
                tree.push_back({token, prevToken, nextToken, lastToken});
            } else {
                tokenBuffer.push_back(token);
            }
            token = lexer.next();
        }

        // generate the asembly code
        auto outputASM = std::ostringstream{};
        outputASM << "section .text\nglobal _start\n\n_start:\n";

        for (auto v : tree) {
            if (v[0].type() == "ADD") {
                if (v[1].type() == "INTEGER")
                    outputASM << "mov eax, " << std::stoi(v[1].lexeme()) << "\n";
                if (v[2].type() == "INTEGER")
                    outputASM << "add eax, " << std::stoi(v[2].lexeme()) << "\n";
            }
        }

        outputASM << "\nmov ebx, eax\nmov eax, 1\nint 80h\n";

        // print the asembly code to a file
        auto outputFile = std::ofstream{argv[2]};
        outputFile << outputASM.str();
        outputFile.close();
    }
}
