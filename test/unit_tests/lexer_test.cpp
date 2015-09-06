/*
Project: OGLA
File: lexer_test.cpp
Author: Leonardo Banderali
Created: August 27, 2015
Last Modified: September 6, 2015

Description: A simple unit test for the lexer.

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

#include <string>
#include <vector>
#include <tuple>

#include "lexer.hpp"

#define BOOST_TEST_MODULE MyTest
#include <boost/test/unit_test.hpp>

//~test subjects~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

using MyLexer = tuc::Lexer<std::string::const_iterator>;    // simplify access to string lexer

//the string to be analyzed
const std::string text{"1+2;\n2-1;\n2*3;\n8/2;\n"};

// a representation of the tokens expected from the lexer
const std::vector<std::tuple<tuc::TokenType, std::string, int>> expected_tokens = {
    std::make_tuple(tuc::TokenType::INTEGER, "1", 0),
    std::make_tuple(tuc::TokenType::ADD, "+", 1),
    std::make_tuple(tuc::TokenType::INTEGER, "2", 2),
    std::make_tuple(tuc::TokenType::SEMICOL, ";", 3),
    std::make_tuple(tuc::TokenType::INTEGER, "2", 5),
    std::make_tuple(tuc::TokenType::SUBTRACT, "-", 6),
    std::make_tuple(tuc::TokenType::INTEGER, "1", 7),
    std::make_tuple(tuc::TokenType::SEMICOL, ";", 8),
    std::make_tuple(tuc::TokenType::INTEGER, "2", 10),
    std::make_tuple(tuc::TokenType::MULTIPLY, "*", 11),
    std::make_tuple(tuc::TokenType::INTEGER, "3", 12),
    std::make_tuple(tuc::TokenType::SEMICOL, ";", 13),
    std::make_tuple(tuc::TokenType::INTEGER, "8", 15),
    std::make_tuple(tuc::TokenType::DIVIDE, "/", 16),
    std::make_tuple(tuc::TokenType::INTEGER, "2", 17),
    std::make_tuple(tuc::TokenType::SEMICOL, ";", 18)
};



//~helpers~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// a macro that generates the message to be printed by a BOOST_CHECK_MESSAGE
#define MAKE_MESSAGE(token, tuple) "expected:{" << static_cast<int>(std::get<0>(tuple)) << "," << std::get<1>(tuple) << std::get<2>(tuple) \
                                << "} got:{" << static_cast<int>(token.type()) << "," << (token.lexeme()) << "," << (token.position()) << "}"



//~tests~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BOOST_AUTO_TEST_CASE( test_lexer_class ) {
    // test setup code
    auto lexer = MyLexer(text.cbegin(), text.cend());
    auto token = tuc::Token{};

    // test code
    for (int i = 0, s = expected_tokens.size(); i < s; i++) {
        token = lexer.current();
        //BOOST_TEST_REQUIRE((!token.empty()));
        //BOOST_CHECK_REQUIRE((!token.empty()));
        //BOOST_TEST(token.type() == std::get<0>(expected_tokens[i]));
        //BOOST_CHECK(token.type() == std::get<0>(expected_tokens[i]));
        BOOST_CHECK_MESSAGE(token.type() == std::get<0>(expected_tokens[i]), MAKE_MESSAGE(token,(expected_tokens[i])));
        //BOOST_TEST(token.lexeme() == std::get<1>(expected_tokens[i]));
        //BOOST_CHECK(token.lexeme() == std::get<1>(expected_tokens[i]));
        BOOST_CHECK_MESSAGE(token.lexeme() == std::get<1>(expected_tokens[i]), MAKE_MESSAGE(token,(expected_tokens[i])));
        //BOOST_TEST(token.position() == std::get<2>(expected_tokens[i]));
        //BOOST_CHECK(token.position() == std::get<2>(expected_tokens[i]));
        BOOST_CHECK_MESSAGE(token.position() == std::get<2>(expected_tokens[i]), MAKE_MESSAGE(token,(expected_tokens[i])));
        lexer.next();
    }
}
