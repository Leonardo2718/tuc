/*
Project: TUC
File: grammar.hpp
Author: Leonardo Banderali
Created: August 31, 2015
Last Modified: September 6, 2015

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

#ifndef TUC_GRAMMAR_HPP
#define TUC_GRAMMAR_HPP

//include standard c++ libraries
#include <string>
#include <vector>
#include <regex>

//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class Rule;
    class Token;

    enum class TokenType {ADD, SUBTRACT, MULTIPLY, DIVIDE, INTEGER, SEMICOL};

    /*################################################################################################################
    ### Here, a grammar is defined as a list of rule lists (a matrix of rules).  Each list in the grammer containes ##
    ### the rules to be used to find the next token; in other words, the next possible set of rules that generate a ##
    ### valid token, given a current state.  Rules contain a `GrammarIndex` that points to the next list of rules   ##
    ### to be used if the given rule geneates a token.  A rule may point to the current list (the list for which    ##
    ### the rule is a member), or it may point to a different list in the grammar.  This way, the grammar defines   ##
    ### the finite-state-machine (FSM) behavior of the lexer.                                                       ##
    ################################################################################################################*/

    using Grammar = std::vector<std::vector<Rule>>;
    using GrammarIndex = int;
}



//~declare classes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class tuc::Rule {    // a class that defines the rules used to find tokens
    public:
        Rule() = default;
        Rule(const TokenType& _type, const std::string& _regex, GrammarIndex _nextRulesIndex)
            : ruleType{_type}, rgx{_regex}, nextRulesIndex{_nextRulesIndex} {}
        /*  constructs a rule with the name `_name` and uses `_regex` as regular expression for searching;
            `_nextRulesIndex` points to the next list of rules to be used */

        TokenType type() const;
        /*  returns the type of the rule (which should also be the type of the token it searches for) */

        std::regex regex() const;
        /*  returns the regular expression used to search for the token */

        GrammarIndex nextRules() const;
        /*  returns the index pointing to the rules to be used after this rule finds a token */

    private:
        TokenType ruleType;
        std::regex rgx;                 // holds the regular expression (regex) used to indentify the token
        GrammarIndex nextRulesIndex = 0;// indexes the next rules to be used for tokenization
};

class tuc::Token {
    public:
        Token() = default;
        Token(const TokenType& _type, std::smatch m, int _pos = -1) : tokenType{_type}, match{m}, pos{_pos} {}

        bool empty() const;
        /*  returns true if token was generated from an empty match */

        TokenType type() const;
        /*  returns the type of the token (which should match the type of the rule used to find it) */

        int position() const;
        /*  returns the position of the token within the alayzed text */

        std::string lexeme() const;
        /*  returns the lexeme for the token; behavior is undefined if token is empty */

    private:
        TokenType tokenType;
        std::smatch match;  // holds the lexem matched associated with the token
        int pos;            // holds the position of the token in the text (-1 is "unkown position")
};

#endif//TUC_GRAMMAR_HPP
