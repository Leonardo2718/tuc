/*
Project: OGLA
File: tuc_test.cpp
Author: Leonardo Banderali
Created: November 2, 2015
Last Modified: November 2, 2015

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

// tuc headers
#include "text_entity.hpp"
#include "lexer.hpp"
#include "syntax_tree.hpp"
#include "asm_generator.hpp"

// testing framework
#define BOOST_TEST_MODULE TUC_tests
#include <boost/test/unit_test.hpp>

// c++ standard libraries
#include <string>
#include <vector>
#include <tuple>

using namespace tuc;



//~expected values and stubs~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const std::string source_file_path = "tuc_program.ul";

const std::vector<Token> expected_tokens = {
    Token{TokenType::LCOMMENT, TextEntity{"// This is a comment!\n", source_file_path, 0, 1, 1}, -1, Associativity::NONE},

    Token{TokenType::INTEGER, TextEntity{"1", source_file_path, 23, 3, 1}, -1, Associativity::NONE},
    Token{TokenType::ADD, TextEntity{"+", source_file_path, 24, 3, 2}, 3, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"2", source_file_path, 25, 3, 3}, -1, Associativity::NONE},
    Token{TokenType::SEMICOL, TextEntity{";", source_file_path, 26, 3, 4}, -1, Associativity::NONE},
    Token{TokenType::LCOMMENT, TextEntity{"// simple expression; result should be 3\n", source_file_path, 31, 3, 9}, -1, Associativity::NONE},

    Token{TokenType::LPAREN, TextEntity{"(", source_file_path, 73, 5, 1}, -1, Associativity::NONE},
    Token{TokenType::INTEGER, TextEntity{"3", source_file_path, 74, 5, 2}, -1, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 75, 5, 3}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"4", source_file_path, 76, 5, 4}, -1, Associativity::NONE},
    Token{TokenType::ADD, TextEntity{"+", source_file_path, 78, 5, 6}, 3, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"4", source_file_path, 80, 5, 8}, -1, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 81, 5, 9}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"5", source_file_path, 82, 5, 10}, -1, Associativity::NONE},
    Token{TokenType::RPAREN, TextEntity{")", source_file_path, 83, 5, 11}, -1, Associativity::NONE},

    Token{TokenType::DIVIDE, TextEntity{"/", source_file_path, 84, 5, 12}, 4, Associativity::LEFT},

    Token{TokenType::LPAREN, TextEntity{"(", source_file_path, 85, 5, 13}, -1, Associativity::NONE},
    Token{TokenType::INTEGER, TextEntity{"2", source_file_path, 86, 5, 14}, -1, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 87, 5, 15}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"3", source_file_path, 88, 5, 16}, -1, Associativity::NONE},
    Token{TokenType::SUBTRACT, TextEntity{"-", source_file_path, 90, 5, 18}, 3, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"1", source_file_path, 92, 5, 20}, -1, Associativity::NONE},
    Token{TokenType::MULTIPLY, TextEntity{"*", source_file_path, 93, 5, 21}, 4, Associativity::LEFT},
    Token{TokenType::INTEGER, TextEntity{"2", source_file_path, 94, 5, 22}, -1, Associativity::NONE},
    Token{TokenType::RPAREN, TextEntity{")", source_file_path, 95, 5, 23}, -1, Associativity::NONE},

    Token{TokenType::SEMICOL, TextEntity{";", source_file_path, 96, 5, 24}, -1, Associativity::NONE},
    Token{TokenType::LCOMMENT, TextEntity{"// complex expression; result should be 8\n", source_file_path, 101, 5, 29}, -1, Associativity::NONE}
};



//~test cases~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


BOOST_AUTO_TEST_CASE(lexer_test) {
    auto actual_tokens = tuc::lex_analyze(source_file_path);
    BOOST_TEST(actual_tokens.size() == expected_tokens.size());
    for (int i = 0, l = expected_tokens.size(); i < l; i++) {
        BOOST_TEST_CONTEXT("token index: " << i) {
            BOOST_TEST(actual_tokens[i].valid() == expected_tokens[i].valid());
            BOOST_TEST(actual_tokens[i].lexeme() == expected_tokens[i].lexeme(), "[\"" << actual_tokens[i].lexeme() << "\" != \"" << expected_tokens[i].lexeme() << "\"]");
            //BOOST_TEST(actual_tokens[i].type() == expected_tokens[i].type(), "[" << static_cast<char>(actual_tokens[i].type()) << " != " << static_cast<char>(expected_tokens[i].type()) << "]");
            BOOST_TEST(actual_tokens[i].text().position() == expected_tokens[i].text().position());
            BOOST_TEST(actual_tokens[i].text().line() == expected_tokens[i].text().line());
            BOOST_TEST(actual_tokens[i].text().column() == expected_tokens[i].text().column());
            BOOST_TEST(actual_tokens[i].is_operator() == expected_tokens[i].is_operator());
            BOOST_TEST(actual_tokens[i].precedence() == expected_tokens[i].precedence());
            //BOOST_TEST(actual_tokens[i].fixity() == expected_tokens[i].fixity());
        }
    }
}
