/*
Project: TUC
File: lexer.hpp
Author: Leonardo Banderali
Created: August 21, 2015
Last Modified: August 24, 2015

Description:
    TUC is a simple, experimental compiler intended for learning and experimenting.
    It is not designed to have any useful purpose other than being a way to learn
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

namespace tuc {
    template<typename BidirectionalIterator> class Lexer;
}

template<typename BidirectionalIterator>
class tuc::Lexer {
    template <typename NextStateType> class BasicRule;
    using Rule = BasicRule<int>;
    using RuleList = std::vector<Rule>;
    public:
        class Token {
            public:
                Token() {}
                Token(const std::string& _name, std::smatch m, int _offset = 0) : ruleName{_name}, match{m}, offset{_offset} {}

                std::string name() const {
                    return ruleName;
                }

                std::string lexeme() const {
                    if (match.empty())
                        return std::string();
                    else
                        return match.str();
                }

            private:
                std::string ruleName;
                std::smatch match;  // holds the lexem matched associated with the token
                int offset = -1;    // holds the offset for the token position
        };

        Lexer(const BidirectionalIterator& first, const BidirectionalIterator& last, const std::vector<RuleList>& _rules)
            : rules{_rules}, beginning{first}, currentPosition{first}, end{last} {}

        Token current();
        /*  returns the token that was last generated */

        Token next();
        /*  generates and returns the next token */

    private:
        template <typename NextStateType>
        class BasicRule {    // a class that defines the rules used to find tokens
            public:
                BasicRule() {}
                BasicRule(const std::string& _name, const std::string& _regex)
                    : ruleName{_name}, rgx{_regex} {}
                BasicRule(const std::string& _name, const std::string& _regex, NextStateType _nState)
                    : ruleName{_name}, rgx{_regex}, nState{_nState} {}
                /*  constructs a rule with the name `_name` and uses `_regex` as regular expression for matching */

                std::string name() const {
                    return ruleName;
                }

                std::regex regex() const {
                    return rgx;
                }

                NextStateType nextState() const {
                    return nState;
                }

            private:
                std::string ruleName;
                std::regex rgx;                 // holds the regular expression (regex) used to indentify the token
                const NextStateType& nState;    // points to (but does not own) the next rules to be used for tokenization
        };

        struct TokenRulePair {
            Token token;
            Rule rule;
        };

        std::vector<RuleList> rules;

        const BidirectionalIterator beginning;
        const BidirectionalIterator end;
        BidirectionalIterator currentPosition;

        Token currentToken;
        int currentRules;

        /*
        - returns the first token identified and its corresponding rule
        - `first` is an iterator (prefer const_iterator) pointing to the first character of the text to be analyzed
        - `last` is an iterator (prefer const_iterator) pointing to one character past the end of the text to be analyzed
        - `rules` is the list of rules checked when looking for the first token
        - `offset` is the offset from the start of the string at which to begin looking for a token
        */
        //template<class BidirectionalIterator>
        TokenRulePair firstToken(BidirectionalIterator first, BidirectionalIterator last, const RuleList& rules) {
            Token token;
            Rule rule;
            int tokenPosition = -1;
            if (first < last) {
                std::smatch m;
                for (auto r: rules) {
                    if (std::regex_search(first, last, m, r.regex()) && (m.position() < tokenPosition || tokenPosition < 0)) {
                        token = Token(r, m);
                        rule = r;
                        tokenPosition = m.position();
                    }
                }
            }

            return TokenRulePair{token, rule};
        }
};

#endif//TUC_LEXER_HPP
