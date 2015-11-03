/*
Project: TUC
File: lexer.hpp
Author: Leonardo Banderali
Created: August 21, 2015
Last Modified: November 2, 2015

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
#include "u_language.hpp"

// standard libraries
#include <string>
#include <regex>
#include <fstream>
#include <utility>

//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    template <typename RandomAccessIterator>
    std::vector<tuc::Token> lex_analyze(RandomAccessIterator first, RandomAccessIterator last);
    /*  analyze the input text and returns it as a list of tokens */

    std::vector<tuc::Token> lex_analyze(const std::string& filePath);
    /*  analyze an input file and returns its contents as a list of tokens */
}



//~declare and implement template classes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
analyze the input text and returns it as a list of tokens
*/
template <typename RandomAccessIterator>
std::vector<tuc::Token> tuc::lex_analyze(RandomAccessIterator first, RandomAccessIterator last) {
    std::vector<tuc::Token> tokenList;
    RandomAccessIterator currentPosition = first;
    auto ruleListIndex = 0;
    int l = 1;
    int c = 1;

    while (currentPosition < last) {
        Rule rule;
        std::smatch firstMatch;
        std::smatch m;
        for (auto r : u_lexer_grammar[ruleListIndex]) {
            if (std::regex_search(currentPosition, last, m, r.regex()) && (firstMatch.empty() || m.position() < firstMatch.position() )) {
                firstMatch = std::move(m);
                rule = std::move(r);
            }
        }

        if (firstMatch.empty()) {
            break;
        } else {
            /*tokenList.push_back(Token{rule, firstMatch, currentPosition - first + firstMatch.position()});
            ruleListIndex = rule.nextRules();
            currentPosition += firstMatch.position() + firstMatch.length();*/
            for (int i = firstMatch.position() + firstMatch.length() - 1; i >= 0; i--) {
                auto c = *currentPosition;
                if (c == '\n') {
                    l++;
                    c = 1;
                }
                else {
                    c++;
                }
                currentPosition++;
            }
            tokenList.push_back(Token{TextEntity{firstMatch.str(), "", currentPosition - firstMatch.length() - first, l, c}, rule});
            ruleListIndex = rule.nextRules();
        }
    }

    return tokenList;
}

/*
analyze an input file and returns its contents as a list of tokens
*/
std::vector<tuc::Token> tuc::lex_analyze(const std::string& filePath) {
    auto inputFile = std::ifstream{filePath};
    std::stringbuf sb;
    inputFile.get(sb, static_cast<char>(-1)); // read the entire file
    inputFile.close();

    const auto fileText = sb.str();
    auto first = fileText.cbegin();
    auto last = fileText.cend();
    auto currentPosition = first;
    std::vector<tuc::Token> tokenList;
    auto ruleListIndex = 0;
    int l = 1;
    int c = 1;

    while (currentPosition < last) {
        Rule rule;
        std::smatch firstMatch;
        std::smatch m;
        for (auto r : u_lexer_grammar[ruleListIndex]) {
            if (std::regex_search(currentPosition, last, m, r.regex()) && (firstMatch.empty() || m.position() < firstMatch.position() )) {
                firstMatch = std::move(m);
                rule = std::move(r);
            }
        }

        if (firstMatch.empty()) {
            break;
        } else {
            for (int i = firstMatch.position() - 1; i >= 0; i--) {
                auto character = *currentPosition;
                if (character == '\n') {
                    l++;
                    c = 1;
                }
                else {
                    c++;
                }
                currentPosition++;
            }
            tokenList.push_back(Token{TextEntity{firstMatch.str(), filePath, currentPosition - first, l, c}, rule});
            for (int i = firstMatch.length() - 1; i >= 0; i--) {
                auto character = *currentPosition;
                if (character == '\n') {
                    l++;
                    c = 1;
                }
                else {
                    c++;
                }
                currentPosition++;
            }
            ruleListIndex = rule.nextRules();
        }
    }

    return tokenList;
}

#endif//TUC_LEXER_HPP
