/*
Project: TUC
File: asm_generator.cpp
Author: Leonardo Banderali
Created: October 6, 2015
Last Modified: July 2, 2016

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2016 Leonardo Banderali

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
#include "asm_generator.hpp"

// standard libraries
#include <sstream>
#include <iostream>
#include <deque>
#include <vector>
#include <unordered_map>
#include <algorithm>



/*
generates assembly code from a syntax tree and symbol table
*/
std::string tuc::gen_expr_asm(SyntaxNode* node, const SymbolTable& symTable) {
    using NodeType = tuc::NodeType;
    auto outputASM = std::stringstream{};
    auto firstOperand = node->child(0);
    auto secondOperand = node->child(1);
    //auto firstIsOperator = firstOperand->is_operator();
    //auto secondIsOperator = secondOperand->is_operator();
    auto firstIsOperator = is_highorder_op(firstOperand->type());
    auto secondIsOperator = is_highorder_op(secondOperand->type());
    auto firstIsLiteral = (firstOperand->type() == NodeType::INTEGER);
    auto secondIsLiteral = (secondOperand->type() == NodeType::INTEGER);

    if (firstIsOperator && secondIsOperator) {
        outputASM << gen_expr_asm(secondOperand, symTable); // evaluate the right hand side first so that the result
        outputASM << "push eax\n";                          //   from the left hand side ends up in eax. This makes it
        outputASM << gen_expr_asm(firstOperand, symTable);  //   easy to ensures that the result of the current
        outputASM << "pop ebx\n";                           //   operation also ends up in eax.

        if (node->type() == NodeType::ADD)
            outputASM << "add eax, ebx\n";
        else if (node->type() == NodeType::SUBTRACT)
            outputASM << "sub eax, ebx\n";
        else if (node->type() == NodeType::MULTIPLY)
            outputASM << "imul eax, ebx\n";
        else if (node->type() == NodeType::DIVIDE)
            outputASM << "idiv ebx\n";

    } else if (firstIsLiteral && secondIsOperator) {
        outputASM << gen_expr_asm(secondOperand, symTable); // result is put in eax so must be moved to ebx for
                                                            //   some instructions

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
        outputASM << gen_expr_asm(firstOperand, symTable);

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

    return outputASM.str();
}

/*
generates assembly code from intermediate representation
*/
std::string tuc::generate_asm(const IntermediateRepresentation& ir) {
    auto outputASM = std::vector<std::string>{};
    //auto poped_regs = std::deque<std::string>{};
    auto reg_stack = std::deque<std::string>{};
    auto mapped_regs = std::unordered_map<std::string, std::string>{};
    auto mapped_vars = std::unordered_map<std::string, unsigned int>{};
    auto var_map = std::unordered_map<std::string, std::string>{};

    for (auto op = ir.cbegin(), end_op = ir.cend(); op != end_op; ++op) {
        using OpType = Operation::OperationType;
        using ValKind = Value::ValueKind;
        auto instruction = std::stringstream{};

        auto src1 = op->source_one();
        auto src2 = op->source_two();

        if (op->operation() == OpType::ADD) {
            if (src1.kind() == ValKind::Literal && src2.kind() == ValKind::Literal) {
                instruction << "mov" << " " << "eax" << " " << src1.value();
                outputASM.push_back(instruction.str());

                instruction.str(std::string{});

                instruction << "add" << " " << "eax" << " " << src2.value();
                outputASM.push_back(instruction.str());
                var_map[op->destination().value()] = "eax";
            }
            else if  (src1.kind() == ValKind::Variable && src2.kind() == ValKind::Literal) {
                instruction << "add" << " " << var_map.at(src1.value()) << " " << src2.value();
                outputASM.push_back(instruction.str());
                var_map[op->destination().value()] = "eax";
            }
        }

        auto next_op = op + 1;
        if (next_op != end_op) {
            auto destName = op->destination().value();
            if (next_op->source_one().value() != destName && next_op->source_two().value() != destName) {
                instruction.str(std::string{});
                instruction << "push" << " " << var_map.at(destName);
                outputASM.push_back(instruction.str());
            }
        }
    }

    auto output = std::stringstream{};
    for (auto&& i : outputASM) {
        output << i << "\n";
    }
    return output.str();
}
