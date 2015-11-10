/*
Project: OGLA
File: tuc_unit_tests.hpp
Author: Leonardo Banderali
Created: November 8, 2015
Last Modified: November 10, 2015

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

#ifndef TUC_UNIT_TESTS_HPP
#define TUC_UNIT_TESTS_HPP

// tuc headers
#include "text_entity.hpp"
#include "lexer.hpp"
#include "syntax_tree.hpp"
#include "asm_generator.hpp"

// testing framework
//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MODULE TUC_tests
//#include <boost/test/unit_test.hpp>

// c++ standard libraries
#include <string>
#include <vector>
#include <memory>
#include <utility>

using namespace tuc;



//~expected values and stubs~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const std::string source_file_path = "arithmetic_program.ul";

const std::vector<Token> expected_tokens = {
    Token{TokenType::LCOMMENT, TextEntity{"// This is a comment!\n", source_file_path, 0, 1, 1}, -1, Associativity::NONE},

    Token{TokenType::INTEGER, TextEntity{"1", source_file_path, 23, 3, 1}, 20, Associativity::NONE},
    Token{TokenType::ADD, TextEntity{"+", source_file_path, 24, 3, 2}, 3, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"2", source_file_path, 25, 3, 3}, 20, Associativity::NONE},
    Token{TokenType::SEMICOL, TextEntity{";", source_file_path, 26, 3, 4}, -1, Associativity::NONE},
    Token{TokenType::LCOMMENT, TextEntity{"// simple expression; result should be 3\n", source_file_path, 31, 3, 9}, -1, Associativity::NONE},
    Token{TokenType::LCOMMENT, TextEntity{"// complex expression; result should be 8\n", source_file_path, 73, 5, 1}, -1, Associativity::NONE},

    Token{TokenType::LPAREN, TextEntity{"(", source_file_path, 115, 6, 1}, -1, Associativity::NONE},
    Token{TokenType::INTEGER, TextEntity{"3", source_file_path, 116, 6, 2}, 20, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 117, 6, 3}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"4", source_file_path, 118, 6, 4}, 20, Associativity::NONE},
    Token{TokenType::ADD, TextEntity{"+", source_file_path, 120, 6, 6}, 3, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"4", source_file_path, 122, 6, 8}, 20, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 123, 6, 9}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"5", source_file_path, 124, 6, 10}, 20, Associativity::NONE},
    Token{TokenType::RPAREN, TextEntity{")", source_file_path, 125, 6, 11}, -1, Associativity::NONE},

    Token{TokenType::DIVIDE, TextEntity{"/", source_file_path, 126, 6, 12}, 4, Associativity::LEFT},

    Token{TokenType::LPAREN, TextEntity{"(", source_file_path, 127, 6, 13}, -1, Associativity::NONE},
    Token{TokenType::INTEGER, TextEntity{"2", source_file_path, 128, 6, 14}, 20, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 129, 6, 15}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"3", source_file_path, 130, 6, 16}, 20, Associativity::NONE},
    Token{TokenType::SUBTRACT, TextEntity{"-", source_file_path, 132, 6, 18}, 3, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"1", source_file_path, 134, 6, 20}, 20, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 135, 6, 21}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"2", source_file_path, 136, 6, 22}, 20, Associativity::NONE},
    Token{TokenType::RPAREN, TextEntity{")", source_file_path, 137, 6, 23}, -1, Associativity::NONE},

    Token{TokenType::SEMICOL, TextEntity{";", source_file_path, 138, 6, 24}, -1, Associativity::NONE}
};

std::unique_ptr<SyntaxNode> get_syntax_tree();/* {
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

    return std::move(rootNode);
}*/

#endif//TUC_UNIT_TESTS_HPP
