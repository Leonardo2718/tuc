/*
Project: TUC
File: tuc.cpp
Author: Leonardo Banderali
Created: August 7, 2015
Last Modified: September 2, 2015

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
    if (argc == 2) {
        auto inputFile = std::ifstream{argv[1]};
        std::stringbuf sb;
        inputFile.get(sb, static_cast<char>(-1)); // read the entire file
        inputFile.close();
        const auto fileText = sb.str();

        auto uGrammar = tuc::Grammar{
            {
                tuc::Rule{"ADD", "\\+", 0},
                tuc::Rule{"INTEGER", "\\d+", 0},
                tuc::Rule{"SEMICOL", ";", 0}
            }
        };

        auto tree = std::vector<std::vector<tuc::Token>>{}; // a very basic Abstract Syntax Tree (AST)
        auto uLexer = tuc::Lexer<std::string::const_iterator>{fileText.cbegin(), fileText.cend(), uGrammar};
        auto tokenBuffer = std::vector<tuc::Token>{};       // a buffer to store un processed tokens
        auto token = uLexer.current();

        while(!token.empty()) {
            if (token.name() == "ADD") {
                auto prevToken = tokenBuffer.back();
                tokenBuffer.pop_back();
                auto nextToken = uLexer.next();
                tree.push_back({token, prevToken, nextToken});
            } else {
                tokenBuffer.push_back(token);
            }
            token = uLexer.next();
        }

        for (auto v : tree) {
            std::cout << v[0].name() << " " << v[1].lexeme() << " " << v[2].lexeme() << std::endl;
        }
    }
}
