/*
Project: OGLA
File: lexer_test.cpp
Author: Leonardo Banderali
Created: August 27, 2015
Last Modified: August 27, 2015

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

//#include "../include/ogla/ogla.hpp"
#include "lexer.hpp"

#define BOOST_TEST_MODULE MyTest
#include <boost/test/included/unit_test.hpp>

//~test subjects~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

using MyLexer = tuc::Lexer<std::string::const_iterator>;

const auto rules = std::vector<MyLexer::RuleList>{
    {MyLexer::make_rule("foo_rule", "foo", 0),
     MyLexer::make_rule("bar_rule", "\\bbar\\b", 0),
     MyLexer::make_rule("quux_rule", "\\bqu+x\\b", 0),
     MyLexer::make_rule("quick_rule", "\\bquick\\b", 0),
     MyLexer::make_rule("c_rule", "\\b[A-Za-z]+c[A-Za-z]+\\b", 0),
     MyLexer::make_rule("str_rule", "\"", 1)},

    {MyLexer::make_rule("escape_rule", "\\\\.", 1),
     MyLexer::make_rule("end_str_rule", "\"", 0)}
};

const std::string text{"The quick brown fox jumps over the lazy dog.\n"
                 "foo bar quux\n"
                 "gosofooeiowe secbarsde qux quuuuuuuuuux\n"
                 "This is \"an \\t attempt\" to parse a string\n"};
//const ogla::Grammar grammar = ogla::Grammar::load();
//const std::vector<std::tuple<std::string, int, std::string>> expected_tokens = {
const std::vector<std::tuple<std::string, std::string, int>> expected_tokens = {
    std::make_tuple("quick_rule", "quick", 4),
    std::make_tuple("foo_rule", "foo", 45),
    std::make_tuple("bar_rule", "bar", 49),
    std::make_tuple("quux_rule", "quux", 53),
    std::make_tuple("foo_rule", "foo", 62),
    std::make_tuple("c_rule", "secbarsde", 71),
    std::make_tuple("quux_rule", "qux", 81),
    std::make_tuple("quux_rule", "quuuuuuuuuux", 85),
    std::make_tuple("str_rule", "\"", 106),
    std::make_tuple("escape_rule", "\\t", 110),
    std::make_tuple("end_str_rule", "\"", 120)
};



//~helpers~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
Tests for equallity between two values; prints a friendly error message if test fails.
*/
/*template<typename T>
void eq_test(const T& a, const T& b, const std::string& lable, int index) {
    BOOST_CHECK_MESSAGE(a == b, "[" << index << "]" << lable << a << ", expected: " << b);
}

void test_lexeme(const ogla::Token& token, const std::tuple<std::string, int, std::string>& exp, int index) {
    eq_test(token.lexeme(), std::get<0>(exp), " lexeme: ", index);
}

void test_position(const ogla::Token& token, const std::tuple<std::string, int, std::string>& exp, int index) {
    eq_test(token.position(), std::get<1>(exp), " position: ", index);
}

void test_name(const ogla::Token& token, const std::tuple<std::string, int, std::string>& exp, int index) {
    eq_test(token.name(), std::get<2>(exp), " name: ", index);
}*/

#define MAKE_MESSAGE(token, tuple) "expected:{" << (token.name()) << "," << (token.lexeme()) \
                                                << "} got:{" << std::get<0>(tuple) << "," << std::get<1>(tuple) << "}"



//~tests~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BOOST_AUTO_TEST_CASE( test_lexer_class ) {
    // pre-test code
    /*ogla::TokenList tokens = ogla::analyze(text, grammar);

    // run test
    BOOST_CHECK_MESSAGE(tokens.size() == expected_tokens.size(),
                        "token count: " << tokens.size() << ", expected: " << expected_tokens.size());
    for (int i = 0, s = tokens.size(); i < s; i++) {
        test_lexeme(tokens.at(i), expected_tokens.at(i), i);
        test_position(tokens.at(i), expected_tokens.at(i), i);
        test_name(tokens.at(i), expected_tokens.at(i), i);
    }*/
    // test setup code
    auto lexer = MyLexer(text.cbegin(), text.cend(), rules);
    auto token = tuc::Token{};

    // test code
    for (int i = 0, s = expected_tokens.size(); i < s; i++) {
        token = lexer.next();
        //BOOST_TEST_REQUIRE((!token.empty()));
        //BOOST_CHECK_REQUIRE((!token.empty()));
        //BOOST_TEST(token.name() == std::get<0>(expected_tokens[i]));
        //BOOST_CHECK(token.name() == std::get<0>(expected_tokens[i]));
        BOOST_CHECK_MESSAGE(token.name() == std::get<0>(expected_tokens[i]), MAKE_MESSAGE(token,(expected_tokens[i])));
        //BOOST_TEST(token.lexeme() == std::get<1>(expected_tokens[i]));
        //BOOST_CHECK(token.lexeme() == std::get<1>(expected_tokens[i]));
        BOOST_CHECK_MESSAGE(token.lexeme() == std::get<1>(expected_tokens[i]), MAKE_MESSAGE(token,(expected_tokens[i])));
    }
}
