/*
Project: TUC
File: tuc.cpp
Author: Leonardo Banderali
Created: August 7, 2015
Last Modified: September 6, 2015

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
#include "syntax_tree.hpp"

int main(int argc, char** argv) {
    if (argc == 3) {
        // read input file
        auto inputFile = std::ifstream{argv[1]};
        std::stringbuf sb;
        inputFile.get(sb, static_cast<char>(-1)); // read the entire file
        inputFile.close();
        const auto fileText = sb.str();

        // create lexer instance for the text
        auto lexer = tuc::make_lexer(fileText.cbegin(), fileText.cend());
        auto tokenBuffer = std::vector<tuc::Token>{};       // a buffer to store un processed tokens
        auto bufferFlag = true;
        auto tree = tuc::SyntaxTree{};
        auto syntaxNode = tree.root();
        auto token = lexer.current();

        // parse the program into a styntax tree using the output from the lexer
        while(!token.empty()) {
            if (token.type() == tuc::TokenType::ADD || token.type() == tuc::TokenType::SUBTRACT ||
              token.type() == tuc::TokenType::MULTIPLY || token.type() == tuc::TokenType::DIVIDE) {
                syntaxNode = tree.appendChild(syntaxNode, token);
                tree.appendChild(syntaxNode, tokenBuffer.back());
                tokenBuffer.pop_back();
                bufferFlag = false;
            } else if (token.type() == tuc::TokenType::SEMICOL) {
                syntaxNode = tree.parent(syntaxNode);
                bufferFlag = true;
            } else if (bufferFlag) {
                tokenBuffer.push_back(token);
            } else {
                tree.appendChild(syntaxNode, token);
            }
            token = lexer.next();
        }

        // generate the asembly code
        auto outputASM = std::ostringstream{};
        outputASM << "section .text\nglobal _start\n\n_start:\n";

        for (auto n : tree.children(tree.root())) {
            token = n.token();
            auto operands = tree.children(n);
            if (token.type() == tuc::TokenType::ADD) {
                if (operands[0].token().type() == tuc::TokenType::INTEGER)
                    outputASM << "mov eax, " << std::stoi(operands[0].token().lexeme()) << "\nadd eax, ";
                if (operands[1].token().type() == tuc::TokenType::INTEGER)
                    outputASM << std::stoi(operands[1].token().lexeme()) << "\n";
            }
            else if (token.type() == tuc::TokenType::SUBTRACT) {
                if (operands[0].token().type()  == tuc::TokenType::INTEGER)
                    outputASM << "mov eax, " << std::stoi(operands[0].token().lexeme()) << "\nsub eax, ";
                if (operands[1].token().type() == tuc::TokenType::INTEGER)
                    outputASM << std::stoi(operands[1].token().lexeme()) << "\n";
            }
            else if (token.type() == tuc::TokenType::MULTIPLY) {
                if (operands[0].token().type()  == tuc::TokenType::INTEGER)
                    outputASM << "mov eax, " << std::stoi(operands[0].token().lexeme()) << "\nimul eax, ";
                if (operands[1].token().type() == tuc::TokenType::INTEGER)
                    outputASM << std::stoi(operands[1].token().lexeme()) << "\n";
            }
            else if (token.type() == tuc::TokenType::DIVIDE) {
                if (operands[0].token().type()  == tuc::TokenType::INTEGER)
                    outputASM << "mov eax, " << std::stoi(operands[0].token().lexeme()) << "\n";
                if (operands[1].token().type() == tuc::TokenType::INTEGER)
                    outputASM << "mov ebx, " << std::stoi(operands[1].token().lexeme()) << "\nidiv ebx\n";
                continue;
            }
        }

        outputASM << "\nmov ebx, eax\nmov eax, 1\nint 80h\n";

        // print the asembly code to a file
        auto outputFile = std::ofstream{argv[2]};
        outputFile << outputASM.str();
        outputFile.close();
    }
}
