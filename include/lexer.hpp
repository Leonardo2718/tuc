/*
Project: TUC
File: lexer.hpp
Author: Leonardo Banderali
Created: August 21, 2015
Last Modified: October 6, 2015

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

// project headers
#include "grammar.hpp"

// standard libraries
#include <regex>
#include <utility>

//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    template<typename RandomAccessIterator> class Lexer;
    template<typename RandomAccessIterator>
    Lexer<RandomAccessIterator> make_lexer(RandomAccessIterator first, RandomAccessIterator last);

    template <typename RandomAccessIterator>
    std::vector<tuc::Token> lex_analyze(RandomAccessIterator first, RandomAccessIterator last);
    /*  analyze the input text and returns it as a list of tokens */
}



//~declare and implement template classes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template<typename RandomAccessIterator>
class tuc::Lexer {
    public:
        Lexer(RandomAccessIterator first, RandomAccessIterator last);
        /*  cunstructs a lexer object where `current()` returns the first token found */

        Token current();
        /*  returns the token that was last generated */

        Token next();
        /*  generates and returns the next token */

    private:
        static Grammar grammar;

        RandomAccessIterator beginning;
        RandomAccessIterator end;
        RandomAccessIterator currentPosition;

        Token currentToken;
        GrammarIndex currentRules = 0;    // 0 is the default rule list
};


template<typename RandomAccessIterator>
tuc::Grammar tuc::Lexer<RandomAccessIterator>::grammar = tuc::Grammar{
    {
        Rule{TokenType::LCOMMENT, "//(.*)(\\n|$)", 0},
        Rule{TokenType::ADD, "\\+", 0},
        Rule{TokenType::SUBTRACT, "\\-", 0},
        Rule{TokenType::MULTIPLY, "\\*", 0},
        Rule{TokenType::DIVIDE, "\\/", 0},
        Rule{TokenType::INTEGER, "\\d+", 0},
        Rule{TokenType::LPAREN, "\\(", 0},
        Rule{TokenType::RPAREN, "\\)", 0},
        Rule{TokenType::SEMICOL, ";", 0}
    }
};

/*
cunstructs a lexer object where `current()` returns the first token found
*/
template<typename RandomAccessIterator>
tuc::Lexer<RandomAccessIterator>::Lexer(RandomAccessIterator first, RandomAccessIterator last)
: beginning{first}, end{last}, currentPosition{first} {
    next();
}

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
        }
    }

    if (firstMatch.empty()) {
        currentToken = Token{};
    } else {
        currentToken = Token{rule.type(), firstMatch, currentPosition - beginning + firstMatch.position() };
        currentRules = rule.nextRules();
        currentPosition += firstMatch.position() + firstMatch.length();
    }

    return currentToken;
}


template<typename RandomAccessIterator>
tuc::Lexer<RandomAccessIterator> tuc::make_lexer(RandomAccessIterator first, RandomAccessIterator last) {
    return Lexer<RandomAccessIterator>{first, last};
}



/*
analyze the input text and returns it as a list of tokens
*/
template <typename RandomAccessIterator>
std::vector<tuc::Token> tuc::lex_analyze(RandomAccessIterator first, RandomAccessIterator last) {
    tuc::Grammar grammar = tuc::Grammar{
        {
            Rule{TokenType::LCOMMENT, "//(.*)(\\n|$)", 0},
            Rule{TokenType::ADD, "\\+", 0},
            Rule{TokenType::SUBTRACT, "\\-", 0},
            Rule{TokenType::MULTIPLY, "\\*", 0},
            Rule{TokenType::DIVIDE, "\\/", 0},
            Rule{TokenType::INTEGER, "\\d+", 0},
            Rule{TokenType::LPAREN, "\\(", 0},
            Rule{TokenType::RPAREN, "\\)", 0},
            Rule{TokenType::SEMICOL, ";", 0}
        }
    };

    std::vector<tuc::Token> tokenList;
    RandomAccessIterator currentPosition = first;
    auto ruleListIndex = 0;

    while (currentPosition < last) {
        Rule rule;
        std::smatch firstMatch;
        std::smatch m;
        for (auto r: grammar[ruleListIndex]) {
            if (std::regex_search(currentPosition, last, m, r.regex()) && (firstMatch.empty() || m.position() < firstMatch.position() )) {
                firstMatch = std::move(m);
                rule = std::move(r);
            }
        }

        if (firstMatch.empty()) {
            //tokenList.push_back(Token{});
            break;
        } else {
            tokenList.push_back(Token{rule.type(), firstMatch, currentPosition - first + firstMatch.position()});
            ruleListIndex = rule.nextRules();
            currentPosition += firstMatch.position() + firstMatch.length();
        }
    }

    return tokenList;
}

#endif//TUC_LEXER_HPP
