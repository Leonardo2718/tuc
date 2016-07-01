/*
Project: TUC
File: ir_generator.cpp
Author: Leonardo Banderali
Created: June 25, 2016
Last Modified: July 1, 2016

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

#include "ir_generator.hpp"
#include "node_type.hpp"

// standard libraries
#include <string>
#include <vector>



//~helper functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

auto nodetype_to_operationtype(tuc::NodeType t) -> tuc::Operation::OperationType {
    using OpType = tuc::Operation::OperationType;
    using NType = tuc::NodeType;
    switch (t) {
    case NType::ADD:      return OpType::ADD;
    case NType::SUBTRACT: return OpType::SUB;
    case NType::MULTIPLY: return OpType::MUL;
    case NType::DIVIDE:   return OpType::DIV;
    default: return static_cast<OpType>(-1);
    }
}

void process_expr(tuc::IntermediateRepresentation& ir, const tuc::SyntaxNode* expr_node, const tuc::Value& dest) {
    using namespace tuc;
    using ValueKind = Value::ValueKind;

    auto left_operand = expr_node->child(0);
    auto right_operand = expr_node->child(1);

    auto left_is_int = (left_operand->type() ==  NodeType::INTEGER);
    auto right_is_int = (right_operand->type() ==  NodeType::INTEGER);

    auto op_type = nodetype_to_operationtype(expr_node->type());

    // assume that (left_is_int && right_is_int) is true
    auto left_src = Value{ValueKind::Literal, left_operand->value()};
    auto right_src = Value{ValueKind::Literal, right_operand->value()};

    // if it's not, perform the appropriate correction and evaluate child nodes
    if ((!left_is_int) && right_is_int) {
        // create temporary variable to store result of left operation
        left_src = VariableFactory::get_variable();

        // generate ir for the operation
        process_expr(ir, left_operand, left_src);
    }
    else if (left_is_int && (!right_is_int)) {
        // create temporary variable to store result of right operation
        right_src = VariableFactory::get_variable();

        // generate ir for right operation
        process_expr(ir, right_operand, right_src);
    }
    else if ((!left_is_int) && (!right_is_int)) {
        // create temporary variables to store result of left and right operations
        left_src = VariableFactory::get_variable();
        right_src = VariableFactory::get_variable();

        // generate ir for left and right operations
        process_expr(ir, left_operand, left_src);
        process_expr(ir, right_operand, right_src);
    }

    // add current operation to ir
    ir.push_back(Operation{op_type, dest, left_src, right_src});
}



//~class member implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::unordered_map<std::string, unsigned int> tuc::VariableFactory::variable_list = std::unordered_map<std::string, unsigned int>{};

auto tuc::generate_ir(const SyntaxNode* syntax_tree) -> IntermediateRepresentation {
    IntermediateRepresentation ir;

    const auto child_count = syntax_tree->child_count();
    for (int i = 0; i < child_count; i++) {
        auto node = syntax_tree->child(i);
        auto node_type = node->type();

        if (is_arithmetic(node_type)) {
            auto dest = VariableFactory::get_variable();
            process_expr(ir, node, dest);
        }
        else if (node_type == NodeType::ASSIGN) {
            auto dest = VariableFactory::get_variable(node->child(0)->value());
            process_expr(ir, node->child(1), dest);
        }
    }

    return ir;
}
