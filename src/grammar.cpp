/*
Project: TUC
File: lexer.cpp
Author: Leonardo Banderali
Created: August 31, 2015
Last Modified: October 9, 2015

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

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

#include "grammar.hpp"



/*
returns the type of the rule (which should also be the type of the token it searches for)
*/
tuc::TokenType tuc::Rule::type() const noexcept {
    return ruleType;
}

/*
returns the regular expression used to search for the token
*/
std::regex tuc::Rule::regex() const noexcept {
    return rgx;
}

/*
returns the index pointing to the rules to be used after this rule finds a token
*/
tuc::GrammarIndex tuc::Rule::nextRules() const noexcept {
    return nextRulesIndex;
}

/*
if the token is some sort of operator, returns its precedence (-1 if not an operator)
*/
tuc::Precedence tuc::Rule::precedence() const noexcept {
    return opPred;
}

/*
if the token is some sort of operator, returns its associativity (NONE if not an operator)
*/
tuc::Associativity tuc::Rule::fixity() const noexcept {
    return opFixity;
}



/*  constructs a token from a grammar rule and a rule match */
tuc::Token::Token(const tuc::Rule& _rule, const std::smatch _rmatch, int _pos) {
    Token{_rule.type(), _rmatch, _pos, _rule.precedence(), _rule.fixity()};
}

/*
returns true if token was generated from an empty match
*/
bool tuc::Token::empty() const noexcept {
    return match.empty();
}

/*
returns the type of the token (which should match the name of the rule used to find it)
*/
tuc::TokenType tuc::Token::type() const noexcept {
    return tokenType;
}

/*
returns the position of the token within the alayzed text
*/
int tuc::Token::position() const noexcept {
    return pos;
}

/*
returns the lexeme for the token; behavior is undefined if token is empty
*/
std::string tuc::Token::lexeme() const noexcept {
    return match.str();
}

/*
if the token is some sort of operator, returns its precedence (-1 if not an operator)
*/
tuc::Precedence tuc::Token::precedence() const noexcept {
    return opPred;
}

/*
if the token is some sort of operator, returns its associativity (NONE if not an operator)
*/
tuc::Associativity tuc::Token::fixity() const noexcept {
    return opFixity;
}
