/*
Project: TUC
File: tuc.cpp
Author: Leonardo Banderali
Created: August 7, 2015
Last Modified: October 9, 2015

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
#include <memory>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <list>

// project headers
#include "lexer.hpp"
#include "syntax_tree.hpp"
#include "asm_generator.hpp"



void print_syntax_nodes(std::ostream& os, const std::list<tuc::SyntaxNode*>& nodeQueue) {
    auto queue = std::list<tuc::SyntaxNode*>{};
    for (auto node : nodeQueue) {
        os << node->value() << " ";
        for (int i = 0, count = node->child_count(); i < count; i++)
            queue.push_back(node->child(i));
    }
    os << "\n";
    if (!queue.empty())
        print_syntax_nodes(os, queue);
}

void print_syntax_tree(std::ostream& os, tuc::SyntaxNode* root) {
    auto queue = std::list<tuc::SyntaxNode*>{};
    queue.push_back(root);
    print_syntax_nodes(os, queue);
}



int main(int argc, char** argv) {
    if (argc == 3) {
        // read input file
        auto inputFile = std::ifstream{argv[1]};
        std::stringbuf sb;
        inputFile.get(sb, static_cast<char>(-1)); // read the entire file
        inputFile.close();
        const auto fileText = sb.str();

        // create lexer instance for the text
        auto tokens = tuc::lex_analyze(fileText.cbegin(), fileText.cend());

        // generate a syntax tree
        auto syntaxTreeRoot = std::make_unique<tuc::SyntaxNode>(tuc::SyntaxNode::NodeType::UNKNOWN);
        auto symbolTable = tuc::SymbolTable{};
        std::tie(syntaxTreeRoot, symbolTable) = tuc::gen_syntax_tree(tokens);

        // generate the asembly code
        auto outputASM = std::ostringstream{};
        outputASM << "section .text\nglobal _start\n\n_start:\n";

        for (int i = 0, count = syntaxTreeRoot->child_count(); i < count; i++) {
            auto n = syntaxTreeRoot->child(i);
            //print_syntax_tree(std::cout, n);  // useful for debugging
            outputASM << tuc::gen_expr_asm(n, symbolTable);
        }

        outputASM << "\nmov ebx, eax\nmov eax, 1\nint 80h\n";

        // print the asembly code to a file
        auto outputFile = std::ofstream{argv[2]};
        outputFile << outputASM.str();
        outputFile.close();
    }
}
