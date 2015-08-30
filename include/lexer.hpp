/*
Project: TUC
File: lexer.hpp
Author: Leonardo Banderali
Created: August 21, 2015
Last Modified: August 29, 2015

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

#ifndef TUC_LEXER_HPP
#define TUC_LEXER_HPP

//include standard c++ libraries
#include <string>
#include <vector>
#include <regex>
#include <utility>

//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class Rule;
    class Token;
    template<typename RandomAccessIterator> class Lexer;

    using RuleList = std::vector<Rule>;
    using RulesIndex = int;

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
        Rule(const std::string& _name, const std::string& _regex, GrammarIndex _nextRulesIndex)
            : ruleName{_name}, rgx{_regex}, nextRulesIndex{_nextRulesIndex} {}
        /*  constructs a rule with the name `_name` and uses `_regex` as regular expression for searching;
            `_nextRulesIndex` points to the next list of rules to be used */

        std::string name() const;
        /*  returns the name of the rule (which should also be the name of the token it searches for) */

        std::regex regex() const;
        /*  returns the regular expression used to search for the token */

        GrammarIndex nextRules() const;
        /*  returns the index pointing to the rules to be used after this rule finds a token */

    private:
        std::string ruleName;
        std::regex rgx;                 // holds the regular expression (regex) used to indentify the token
        GrammarIndex nextRulesIndex = 0;// indexes the next rules to be used for tokenization
};

class tuc::Token {
    public:
        Token() = default;
        Token(const std::string& _name, std::smatch m, int _offset = 0) : ruleName{_name}, match{m}, offset{_offset} {}

        std::string name() const;
        /*  returns the name of the token (which should match the name of the rule used to find it) */

        std::string lexeme() const;
        /*  returns the lexeme for the token */

    private:
        std::string ruleName;
        std::smatch match;  // holds the lexem matched associated with the token
        int offset = -1;    // holds the offset for the token position
};



//~declare and implement template classes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template<typename RandomAccessIterator>
class tuc::Lexer {
    public:
        Lexer(RandomAccessIterator first, RandomAccessIterator last, const Grammar& _grammar)
            : grammar{_grammar}, beginning{first}, end{last}, currentPosition{first} {}

        Token current();
        /*  returns the token that was last generated */

        Token next();
        /*  generates and returns the next token */

        static Rule make_rule(const std::string& _name, const std::string& _regex, int _nextRulesIndex) {
            return Rule{_name, _regex, _nextRulesIndex};
        }

    private:
        Grammar grammar;

        RandomAccessIterator beginning;
        RandomAccessIterator end;
        RandomAccessIterator currentPosition;

        Token currentToken;
        GrammarIndex currentRules = 0;    // 0 is the default rule list
};

/*
returns the token that was last generated
*/
template<typename RandomAccessIterator>
tuc::Token tuc::Lexer<RandomAccessIterator>::current() {
    return currentToken;
}

/*
generates and returns the next token
*/
template<typename RandomAccessIterator>
tuc::Token tuc::Lexer<RandomAccessIterator>::next() {
    Rule rule;
    std::smatch firstMatch;
    std::smatch m;
    for (auto r: grammar[currentRules]) {
        if (std::regex_search(currentPosition, end, m, r.regex()) && (firstMatch.empty() || m.position() < firstMatch.position() )) {
            firstMatch = std::move(m);
            rule = std::move(r);
            //rule = r;
        }
    }

    if (firstMatch.empty()) {
        currentToken = Token{};
        //currentRules = 0;   // 0 points to the default rules
    } else {
        currentPosition += firstMatch.position() + firstMatch.length();
        currentToken = Token{rule.name(), firstMatch};
        currentRules = rule.nextRules();
    }

    return currentToken;
}

#endif//TUC_LEXER_HPP
