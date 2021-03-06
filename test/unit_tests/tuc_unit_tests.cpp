/*
Project: OGLA
File: tuc_unit_tests.cpp
Author: Leonardo Banderali
Created: November 8, 2015
Last Modified: December 17, 2015

Description: A collection of unit tests for the lexer, syntax tree generator,
    and assembly code generator. These unit tests use the Boos Test framework.

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

#define BOOST_TEST_MODULE TUC_tests
#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "tuc_unit_tests.hpp"

//BOOST_AUTO_TEST_SUITE(__phonny_no_tests_here)

std::unique_ptr<SyntaxNode> get_syntax_tree() {
    auto rootNode = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::PROGRAM);

    auto n1 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::ADD, TextEntity{"+", source_file_path, 24, 3, 2});
    n1->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"1", source_file_path, 23, 3, 1}));
    n1->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"2", source_file_path, 25, 3, 3}));
    rootNode->append_child(std::move(n1));

    n1 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::DIVIDE, TextEntity{"/", source_file_path, 126, 6, 12});

    auto n2 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::ADD, TextEntity{"+", source_file_path, 120, 6, 6});

    auto n3 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::MULTIPLY, TextEntity{"*", source_file_path, 117, 6, 3});
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"3", source_file_path, 116, 6, 2}));
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"4", source_file_path, 118, 6, 4}));
    n2->append_child(std::move(n3));

    n3 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::MULTIPLY, TextEntity{"*", source_file_path, 123, 6, 9});
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"4", source_file_path, 122, 6, 8}));
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"5", source_file_path, 124, 6, 10}));
    n2->append_child(std::move(n3));

    n1->append_child(std::move(n2));

    n2 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::SUBTRACT, TextEntity{"-", source_file_path, 132, 6, 18});

    n3 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::MULTIPLY, TextEntity{"*", source_file_path, 129, 6, 15});
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"2", source_file_path, 128, 6, 14}));
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"3", source_file_path, 130, 6, 16}));
    n2->append_child(std::move(n3));

    n3 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::MULTIPLY, TextEntity{"*", source_file_path, 135, 6, 21});
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"1", source_file_path, 134, 6, 20}));
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::INTEGER, TextEntity{"2", source_file_path, 136, 6, 22}));
    n2->append_child(std::move(n3));

    n1->append_child(std::move(n2));

    rootNode->append_child(std::move(n1));

    n1 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::HASTYPE, TextEntity{":", source_file_path, 185, 9, 12});
    n1->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::IDENTIFIER, TextEntity{"function_a", source_file_path, 174, 9, 1}));

    n2 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::MAPTO, TextEntity{"->", source_file_path, 195, 9, 22});
    n3 = std::make_unique<SyntaxNode>(SyntaxNode::NodeType::TYPE, TextEntity{"int", source_file_path, 191, 9, 18});
    n3->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::TYPE, TextEntity{"int", source_file_path, 187, 9, 14}));
    n2->append_child(std::move(n3));
    n2->append_child(std::make_unique<SyntaxNode>(SyntaxNode::NodeType::TYPE, TextEntity{"int", source_file_path, 198, 9, 25}));

    n1->append_child(std::move(n2));

    rootNode->append_child(std::move(n1));

    return std::move(rootNode);
}

//BOOST_AUTO_TEST_SUITE_END()
