/*
Project: TUC
File: tuc.cpp
Author: Leonardo Banderali
Created: August 7, 2015
Last Modified: September 7, 2015

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



auto isOperator = [&](tuc::SyntaxTree::SyntaxNode n) -> bool {
    return  n.token().type() == tuc::TokenType::ADD || n.token().type() == tuc::TokenType::SUBTRACT ||
            n.token().type() == tuc::TokenType::MULTIPLY || n.token().type() == tuc::TokenType::DIVIDE;
};

auto generateMathASM(std::ostream& outputASM, tuc::SyntaxTree& tree, tuc::SyntaxTree::SyntaxNode n) -> void {
    auto t = n.token();
    auto operands = tree.children(n);
    std::cout << "point 1:" << t.lexeme() << ":" << operands[0].token().lexeme() << ":" << operands[1].token().lexeme() << "\n";
    auto firstIsOperator = isOperator(operands[0]);
    auto secondIsOperator = isOperator(operands[1]);
    auto firstIsLiteral = (operands[0].token().type() == tuc::TokenType::INTEGER);
    auto secondIsLiteral = (operands[1].token().type() == tuc::TokenType::INTEGER);
    if (firstIsOperator && secondIsOperator) {
        generateMathASM(outputASM, tree, operands[1]);  // evaluate the right hand side first so that the result from the left
        outputASM << "push eax\n";                      //  hand side ends up in eax. This makes it easy to ensures that the
        generateMathASM(outputASM, tree, operands[0]);  //  result of the current operation also ends up in eax.
        outputASM << "pop ebx\n";

        if (n.token().type() == tuc::TokenType::ADD)
            outputASM << "add eax, ebx\n";
        else if (n.token().type() == tuc::TokenType::SUBTRACT)
            outputASM << "sub eax, ebx\n";
        else if (n.token().type() == tuc::TokenType::MULTIPLY)
            outputASM << "imul eax, ebx\n";
        else if (n.token().type() == tuc::TokenType::DIVIDE)
            outputASM << "idiv ebx\n";

    } else if (firstIsLiteral && secondIsOperator) {
        generateMathASM(outputASM, tree, operands[1]);   // result is put in eax so must be moved to ebx for some instructions

        if (n.token().type() == tuc::TokenType::ADD) {
            outputASM << "add eax, " << std::stoi(operands[0].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::SUBTRACT) {
            outputASM << "mov ebx, eax\nmov eax, " << std::stoi(operands[0].token().lexeme()) << "\nsub eax, ebx\n";
        } else if (n.token().type() == tuc::TokenType::MULTIPLY) {
            outputASM << "imul eax, " << std::stoi(operands[0].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::DIVIDE) {
            outputASM << "mov ebx, eax\nmov eax, " << std::stoi(operands[0].token().lexeme()) << "\nidiv ebx\n";
        }

    } else if (firstIsOperator && secondIsLiteral) {
        generateMathASM(outputASM, tree, operands[0]);

        if (n.token().type() == tuc::TokenType::ADD) {
            outputASM << "add eax, " << std::stoi(operands[1].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::SUBTRACT) {
            outputASM << "sub eax, " << std::stoi(operands[1].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::MULTIPLY) {
            outputASM << "imul eax, " << std::stoi(operands[1].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::DIVIDE) {
            outputASM << "mov ebx, " << std::stoi(operands[1].token().lexeme()) << "\nidiv ebx\n";
        }

    } else if (firstIsLiteral && secondIsLiteral) {
        if (n.token().type() == tuc::TokenType::ADD) {
            outputASM   << "mov eax, " << std::stoi(operands[0].token().lexeme())
                        << "\nadd eax, " << std::stoi(operands[1].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::SUBTRACT) {
            outputASM   << "mov eax, " << std::stoi(operands[0].token().lexeme())
                        << "\nsub eax, " << std::stoi(operands[1].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::MULTIPLY) {
            outputASM   << "mov eax, " << std::stoi(operands[0].token().lexeme())
                        << "\nimul eax, " << std::stoi(operands[1].token().lexeme()) << "\n";
        } else if (n.token().type() == tuc::TokenType::DIVIDE) {
            outputASM   << "mov eax, " << std::stoi(operands[0].token().lexeme())
                        << "\nmov ebx, " << std::stoi(operands[1].token().lexeme()) << "\nidiv ebx\n";
        }
    }
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
        auto lexer = tuc::make_lexer(fileText.cbegin(), fileText.cend());
        auto tree = tuc::SyntaxTree{};
        auto syntaxNode = tree.root();
        auto rpnExpr = std::vector<tuc::Token>{};
        auto opStack = std::vector<tuc::Token>{};

        /*##########################################################################################################
        ### To parse mathematical expressions, first use a variation of Dijkstra's shunting yard algorithm to     ##
        ### transform the expression into Reverse Polish Notation (RPN).  Once transformed, the expression can be ##
        ### read *right-to-left* (*backwards*) into the syntax tree.  This is because the last operations to be   ##
        ### executed are the ones that must appear at the top of the syntax tree for the expression.  This also   ##
        ### means that the branches (linking to the operator's operands) must also be added right-to-left.        ##
        ###                                                                                                       ##
        ### This top-down approach is neither efficient nor elegant.  So, it will likely be replaced by a         ##
        ### bottom-up parser in the future.                                                                       ##
        ##########################################################################################################*/
        auto token = lexer.current();
        while(!token.empty()) {
            if (token.type() == tuc::TokenType::ADD || token.type() == tuc::TokenType::SUBTRACT) {
                while (!opStack.empty() && (opStack.back().type() == tuc::TokenType::ADD ||
                                            opStack.back().type() == tuc::TokenType::SUBTRACT ||
                                            opStack.back().type() == tuc::TokenType::MULTIPLY ||
                                            opStack.back().type() == tuc::TokenType::DIVIDE) ) {
                    rpnExpr.push_back(opStack.back());
                    opStack.pop_back();
                }
                opStack.push_back(token);
            }
            if (token.type() == tuc::TokenType::MULTIPLY || token.type() == tuc::TokenType::DIVIDE) {
                while (!opStack.empty() && (opStack.back().type() == tuc::TokenType::MULTIPLY ||
                                            opStack.back().type() == tuc::TokenType::DIVIDE) ) {
                    rpnExpr.push_back(opStack.back());
                    opStack.pop_back();
                }
                opStack.push_back(token);
            }
            if (token.type() == tuc::TokenType::INTEGER) {
                rpnExpr.push_back(token);
            }
            if (token.type() == tuc::TokenType::SEMICOL) {
                while (!opStack.empty()) {
                    rpnExpr.push_back(opStack.back());
                    opStack.pop_back();
                }
                syntaxNode = tree.appendChild(syntaxNode, rpnExpr.back());
                rpnExpr.pop_back();
                int intCount = 0;
                std::cout << syntaxNode.token().lexeme() << "\n";
                for (auto t = rpnExpr.crbegin(); t != rpnExpr.crend(); ++t) {
                    std::cout << t->lexeme() << "\n";
                    if (t->type() == tuc::TokenType::ADD || t->type() == tuc::TokenType::SUBTRACT ||
                      t->type() == tuc::TokenType::MULTIPLY || t->type() == tuc::TokenType::DIVIDE) {
                        syntaxNode = tree.prependChild(syntaxNode, *t);
                        intCount = 0;
                    } else if (t->type() == tuc::TokenType::INTEGER) {
                        tree.prependChild(syntaxNode, *t);
                        intCount++;
                        if (intCount > 1)
                            syntaxNode = tree.parent(syntaxNode);
                    }
                }
                rpnExpr.clear();
                std::cout << ";\n";
            }
            token = lexer.next();
        }

        // generate the asembly code
        auto outputASM = std::ostringstream{};
        outputASM << "section .text\nglobal _start\n\n_start:\n";

        for (auto n : tree.children(tree.root())) {
            generateMathASM(outputASM, tree, n);
            /*token = n.token();
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
            }*/
        }

        outputASM << "\nmov ebx, eax\nmov eax, 1\nint 80h\n";

        // print the asembly code to a file
        auto outputFile = std::ofstream{argv[2]};
        outputFile << outputASM.str();
        outputFile.close();
    }
}
