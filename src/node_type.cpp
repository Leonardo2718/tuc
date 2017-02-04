/*
Project: TUC
File: node_type.cpp
Author: Leonardo Banderali
Created: January 8, 2016
Last Modified: January 8, 2016

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
#include "node_type.hpp"

auto tuc::is_literal(NodeType t) -> bool {
    return is_type_in<NodeType::INTEGER, NodeType::FLOAT, NodeType::STRING>(t);
}

auto tuc::is_exp_entity(NodeType t) -> bool {
    return is_type_in<NodeType::IDENTIFIER, NodeType::TYPE>(t) || is_literal(t);
}

auto tuc::is_arithmetic(NodeType t) -> bool {
    return is_type_in<NodeType::ADD, NodeType::SUBTRACT, NodeType::MULTIPLY,NodeType::DIVIDE,
                        NodeType::INTEGER, NodeType::FLOAT>(t);
}

auto tuc::is_highorder_op(NodeType t) -> bool {
    return is_type_in<NodeType::HASTYPE, NodeType::MAPTO, NodeType::ADD, NodeType::SUBTRACT, NodeType::MULTIPLY,
                        NodeType::DIVIDE,NodeType::ASSIGN>(t);
}