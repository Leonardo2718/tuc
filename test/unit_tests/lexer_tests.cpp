/*
Project: OGLA
File: lexer_tests.cpp
Author: Leonardo Banderali
Created: November 2, 2015
Last Modified: January 8, 2016

Description: A collection of unit tests for the lexer, syntax tree generator,
    and assembly code generator. These unit tests use the Boos Test framework.

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

#define BOOST_TEST_DYN_LINK
#include <boost/test/unit_test.hpp>

#include "tuc_unit_tests.hpp"



//~test cases~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BOOST_AUTO_TEST_SUITE(lexer_unit_tests)

BOOST_AUTO_TEST_CASE(lexer_test) {
    auto actual_tokens = tuc::lex_analyze(source_file_path);
    BOOST_TEST(actual_tokens.size() == expected_tokens.size());
    for (int i = 0, l = expected_tokens.size(); i < l; i++) {
        BOOST_TEST_CONTEXT("token index: " << i) {
            BOOST_TEST(actual_tokens[i].valid() == expected_tokens[i].valid());
            BOOST_TEST(actual_tokens[i].lexeme() == expected_tokens[i].lexeme(), "[\"" << actual_tokens[i].lexeme() << "\" != \"" << expected_tokens[i].lexeme() << "\"]");
            //BOOST_TEST(actual_tokens[i].type() == expected_tokens[i].type(), "[" << static_cast<char>(actual_tokens[i].type()) << " != " << static_cast<char>(expected_tokens[i].type()) << "]");
            BOOST_TEST(actual_tokens[i].text().file_path() == expected_tokens[i].text().file_path());
            BOOST_TEST(actual_tokens[i].text().index() == expected_tokens[i].text().index());
            BOOST_TEST(actual_tokens[i].text().line() == expected_tokens[i].text().line());
            BOOST_TEST(actual_tokens[i].text().column() == expected_tokens[i].text().column());
            BOOST_TEST(actual_tokens[i].precedence() == expected_tokens[i].precedence());
            //BOOST_TEST(actual_tokens[i].fixity() == expected_tokens[i].fixity());
        }
    }
}

BOOST_AUTO_TEST_SUITE_END()
