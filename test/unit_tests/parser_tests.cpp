/*
Project: OGLA
File: parser_tests.cpp
Author: Leonardo Banderali
Created: November 8, 2015
Last Modified: January 17, 2016

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

// c++ standard libraries
#include <tuple>
#include <deque>
#include <sstream>



//~test cases~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BOOST_AUTO_TEST_SUITE(parser_tests)

BOOST_AUTO_TEST_CASE(parser_test) {
    auto expectedRoot = get_syntax_tree();
    auto actualRoot = gen_syntax_tree(expected_tokens);

    std::deque<SyntaxNode*> expectedNodes;
    std::deque<SyntaxNode*> actualNodes;

    expectedNodes.push_back(expectedRoot.get());
    actualNodes.push_back(actualRoot.get());
    while(!expectedNodes.empty() && !actualNodes.empty()) {
        auto expectedNode = expectedNodes.front();
        expectedNodes.pop_front();
        auto actualNode = actualNodes.front();
        actualNodes.pop_front();
        std::stringstream context;
        context << "expected node:\n" << expectedNode << "\n    actual node:\n" << actualNode;
        BOOST_TEST_CONTEXT(context.str()) {
            //BOOST_TEST(actualNode->type() == expectedNode->type());
            BOOST_TEST(actualNode->is_operator() == expectedNode->is_operator());
            BOOST_TEST(actualNode->value() == expectedNode->value());
            BOOST_TEST(actualNode->position().file_path() == expectedNode->position().file_path());
            BOOST_TEST(actualNode->position().index() == expectedNode->position().index());
            BOOST_TEST(actualNode->position().line() == expectedNode->position().line());
            BOOST_TEST(actualNode->position().column() == expectedNode->position().column());
            BOOST_TEST(actualNode->child_count() == expectedNode->child_count());
            for (int i = 0, c = expectedNode->child_count(); i < c; i++) {
                expectedNodes.push_back(expectedNode->child(i));
                actualNodes.push_back(actualNode->child(i));
            }
        }
    }

    BOOST_TEST(expectedNodes.empty() == actualNodes.empty());
}

BOOST_AUTO_TEST_SUITE_END()
