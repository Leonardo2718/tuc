/*
Project: TUC
File: grammar.hpp
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

#ifndef TUC_GRAMMAR_HPP
#define TUC_GRAMMAR_HPP

// project headers
#include "node_type.hpp"
#include "text_entity.hpp"

// c++ standard libraries
#include <string>
#include <vector>
#include <regex>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class Rule;
    class Token;

    using Precedence = int;
    enum class Associativity {LEFT, RIGHT, NONE};

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
        Rule(const NodeType& _type, const std::string& _regex, GrammarIndex _nextRulesIndex,
            Precedence _precedence = -1, Associativity _fixity = Associativity::NONE);
        /*  constructs a rule with the name `_name` and uses `_regex` as regular expression for searching;
            `_nextRulesIndex` points to the next list of rules to be used */

        NodeType type() const noexcept;
        /*  returns the type of the rule (which should also be the type of the token it searches for) */

        std::regex regex() const noexcept;
        /*  returns the regular expression used to search for the token */

        GrammarIndex nextRules() const noexcept;
        /*  returns the index pointing to the rules to be used after this rule finds a token */

        Precedence precedence() const noexcept;
        /*  if the token is some sort of operator, returns its precedence (-1 if not an operator) */

        Associativity fixity() const noexcept;
        /*  if the token is some sort of operator, returns its associativity (NONE if not an operator) */

    private:
        NodeType ruleType;
        std::regex rgx;                                 // the regular expression (regex) used to indentify the token
        GrammarIndex nextRulesIndex = 0;                // indexes the next rules to be used for tokenization
        Precedence opPred = -1;                         // precedence if operator
        Associativity opFixity = Associativity::NONE;   // associativity if operator
};

class tuc::Token {
    public:
        /*Token(const NodeType& _type, std::smatch m, int _pos = -1,
            Precedence _precedence = -1, Associativity _fixity = Associativity::NONE);*/
        Token(const NodeType& _type, const TextEntity& _lexemeInfo,
            Precedence _precedence = -1, Associativity _fixity = Associativity::NONE);

        //Token(const Rule& _rule, const std::smatch _rmatch, int _pos);
        /*  constructs a token from a grammar rule and a rule match */
        Token(const TextEntity& _lexemeInfo, const Rule& _rule);

        bool valid() const noexcept;
        /*  returns true if token is valid (was generated from a valid match) */

        NodeType type() const noexcept;
        /*  returns the type of the token (which should match the type of the rule used to find it) */

        int index() const noexcept;
        /*  returns the position of the token within the alayzed text */

        std::string lexeme() const noexcept;
        /*  returns the lexeme for the token; behavior is undefined if token is empty */

        TextEntity text() const noexcept;
        /*  returns the text entity of the lexeme */

        Precedence precedence() const noexcept;
        /*  if the token is some sort of operator, returns its precedence (-1 if not an operator) */

        Associativity fixity() const noexcept;
        /*  if the token is some sort of operator, returns its associativity (NONE if not an operator) */

    private:
        NodeType tokenType;
        TextEntity lexemeInfo;  // holds the token's lexeme and its location
        Precedence opPred = -1;                         // precedence if operator
        Associativity opFixity = Associativity::NONE;   // associativity if operator
};

#endif//TUC_GRAMMAR_HPP
