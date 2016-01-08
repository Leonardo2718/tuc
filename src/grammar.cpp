/*
Project: TUC
File: lexer.cpp
Author: Leonardo Banderali
Created: August 31, 2015
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

#include "grammar.hpp"



/*
constructs a rule with the name `_name` and uses `_regex` as regular expression for searching;
`_nextRulesIndex` points to the next list of rules to be used
*/
tuc::Rule::Rule(const NodeType& _type, const std::string& _regex, GrammarIndex _nextRulesIndex,
    Precedence _precedence, Associativity _fixity)
    : ruleType{_type}, rgx{_regex}, nextRulesIndex{_nextRulesIndex}, opPred{_precedence}, opFixity{_fixity} {}

/*
returns the type of the rule (which should also be the type of the token it searches for)
*/
tuc::NodeType tuc::Rule::type() const noexcept {
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



tuc::Token::Token(const NodeType& _type, const TextEntity& _lexemeInfo, Precedence _precedence, Associativity _fixity)
: tokenType{_type}, lexemeInfo{_lexemeInfo}, opPred{_precedence}, opFixity{_fixity} {}

tuc::Token::Token(const TextEntity& _lexemeInfo, const Rule& _rule)
: tokenType{_rule.type()}, lexemeInfo{_lexemeInfo}, opPred{_rule.precedence()}, opFixity{_rule.fixity()} {}

/*
returns true if token is valid (was generated from a valid match)
*/
bool tuc::Token::valid() const noexcept {
    return !lexemeInfo.text().empty();
}

/*
returns the type of the token (which should match the name of the rule used to find it)
*/
tuc::NodeType tuc::Token::type() const noexcept {
    return tokenType;
}

/*
returns the position of the token within the alayzed text
*/
int tuc::Token::index() const noexcept {
    return lexemeInfo.index();
}

/*
returns the lexeme for the token; behavior is undefined if token is empty
*/
std::string tuc::Token::lexeme() const noexcept {
    return lexemeInfo.text();
}

/*
returns the text entity of the lexeme
*/
tuc::TextEntity tuc::Token::text() const noexcept {
    return lexemeInfo;
}

bool tuc::Token::is_operator() const noexcept {
    return tokenType == NodeType::ADD || tokenType == NodeType::SUBTRACT ||
            tokenType == NodeType::MULTIPLY || tokenType == NodeType::DIVIDE;
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
