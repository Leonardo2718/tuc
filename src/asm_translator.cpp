/*
Project: TUC
File: asm_translator.cpp
Author: Leonardo Banderali
Created: October 6, 2015
Last Modified: October 6, 2015

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

// project headers
#include "asm_translator.hpp"



/*
generates assembly code from a syntax tree
*/
void tuc::gen_asm_expr(std::ostream& outputASM, tuc::SyntaxNode* node) {
    using NodeType = tuc::SyntaxNode::NodeType;
    auto firstOperand = node->child(0);
    auto secondOperand = node->child(1);
    auto firstIsOperator = firstOperand->is_operator();
    auto secondIsOperator = secondOperand->is_operator();
    auto firstIsLiteral = (firstOperand->type() == NodeType::INTEGER);
    auto secondIsLiteral = (secondOperand->type() == NodeType::INTEGER);
    if (firstIsOperator && secondIsOperator) {
        gen_asm_expr(outputASM, secondOperand); // evaluate the right hand side first so that the result from the left
        outputASM << "push eax\n";              //  hand side ends up in eax. This makes it easy to ensures that the
        gen_asm_expr(outputASM, firstOperand);  //  result of the current operation also ends up in eax.
        outputASM << "pop ebx\n";

        if (node->type() == NodeType::ADD)
            outputASM << "add eax, ebx\n";
        else if (node->type() == NodeType::SUBTRACT)
            outputASM << "sub eax, ebx\n";
        else if (node->type() == NodeType::MULTIPLY)
            outputASM << "imul eax, ebx\n";
        else if (node->type() == NodeType::DIVIDE)
            outputASM << "idiv ebx\n";

    } else if (firstIsLiteral && secondIsOperator) {
        gen_asm_expr(outputASM, secondOperand); // result is put in eax so must be moved to ebx for some instructions

        if (node->type() == NodeType::ADD) {
            outputASM << "add eax, " << std::stoi(firstOperand->value()) << "\n";
        } else if (node->type() == NodeType::SUBTRACT) {
            outputASM << "mov ebx, eax\nmov eax, " << std::stoi(firstOperand->value()) << "\nsub eax, ebx\n";
        } else if (node->type() == NodeType::MULTIPLY) {
            outputASM << "imul eax, " << std::stoi(firstOperand->value()) << "\n";
        } else if (node->type() == NodeType::DIVIDE) {
            outputASM << "mov ebx, eax\nmov eax, " << std::stoi(firstOperand->value()) << "\nidiv ebx\n";
        }

    } else if (firstIsOperator && secondIsLiteral) {
        gen_asm_expr(outputASM, firstOperand);

        if (node->type() == NodeType::ADD) {
            outputASM << "add eax, " << std::stoi(secondOperand->value()) << "\n";
        } else if (node->type() == NodeType::SUBTRACT) {
            outputASM << "sub eax, " << std::stoi(secondOperand->value()) << "\n";
        } else if (node->type() == NodeType::MULTIPLY) {
            outputASM << "imul eax, " << std::stoi(secondOperand->value()) << "\n";
        } else if (node->type() == NodeType::DIVIDE) {
            outputASM << "mov ebx, " << std::stoi(secondOperand->value()) << "\nidiv ebx\n";
        }

    } else if (firstIsLiteral && secondIsLiteral) {
        if (node->type() == NodeType::ADD) {
            outputASM   << "mov eax, " << std::stoi(firstOperand->value())
                        << "\nadd eax, " << std::stoi(secondOperand->value()) << "\n";
        } else if (node->type() == NodeType::SUBTRACT) {
            outputASM   << "mov eax, " << std::stoi(firstOperand->value())
                        << "\nsub eax, " << std::stoi(secondOperand->value()) << "\n";
        } else if (node->type() == NodeType::MULTIPLY) {
            outputASM   << "mov eax, " << std::stoi(firstOperand->value())
                        << "\nimul eax, " << std::stoi(secondOperand->value()) << "\n";
        } else if (node->type() == NodeType::DIVIDE) {
            outputASM   << "mov eax, " << std::stoi(firstOperand->value())
                        << "\nmov ebx, " << std::stoi(secondOperand->value()) << "\nidiv ebx\n";
        }
    }
}
