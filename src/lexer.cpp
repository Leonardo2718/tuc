/*
Project: TUC
File: lexer.cpp
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

#include "lexer.hpp"

/*  returns the token that was last generated */
template<typename BidirectionalIterator> typename
tuc::Lexer<BidirectionalIterator>::Token tuc::Lexer<BidirectionalIterator>::current() {
    return currentToken;
}

/*  generates and returns the next token */
template<typename BidirectionalIterator> typename
tuc::Lexer<BidirectionalIterator>::Token tuc::Lexer<BidirectionalIterator>::next() {
    Rule rule;
    std::smatch firstMatch;
    std::smatch m;
    for (auto r: rules[currentRules]) {
        if (std::regex_search(currentPosition, end, m, r.regex()) && (firstMatch.empty() || m.position() < firstMatch.position() )) {
            firstMatch = m;
            rule = r;
        }
    }

    if (firstMatch.empty()) {
        currentToken = Token{};
        currentRules = 0;   // 0 points to the default rules
    } else {
        currentPosition += firstMatch.position();
        currentToken = Token{rule.name(), firstMatch};
        currentRules = rule.nextState();
    }

    return currentToken;
}
