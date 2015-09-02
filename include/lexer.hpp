/*
Project: TUC
File: lexer.hpp
Author: Leonardo Banderali
Created: August 21, 2015
Last Modified: September 2, 2015

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
}



//~declare and implement template classes~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template<typename RandomAccessIterator>
class tuc::Lexer {
    public:
        Lexer(RandomAccessIterator first, RandomAccessIterator last, const Grammar& _grammar);
        /*  cunstructs a lexer object where `current()` returns the first token found */

        Token current();
        /*  returns the token that was last generated */

        Token next();
        /*  generates and returns the next token */

    private:
        Grammar grammar;

        RandomAccessIterator beginning;
        RandomAccessIterator end;
        RandomAccessIterator currentPosition;

        Token currentToken;
        GrammarIndex currentRules = 0;    // 0 is the default rule list
};


/*
cunstructs a lexer object where `current()` returns the first token found
*/
template<typename RandomAccessIterator>
tuc::Lexer<RandomAccessIterator>::Lexer(RandomAccessIterator first, RandomAccessIterator last, const Grammar& _grammar)
: grammar{_grammar}, beginning{first}, end{last}, currentPosition{first} {
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
        currentPosition += firstMatch.position() + firstMatch.length();
        currentToken = Token{rule.name(), firstMatch};
        currentRules = rule.nextRules();
    }

    return currentToken;
}

#endif//TUC_LEXER_HPP
