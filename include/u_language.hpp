/*
Project: TUC
File: u_language.hpp
Author: Leonardo Banderali
Created: November 2, 2015
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

#ifndef TUC_U_LANGUAGE_HPP
#define TUC_U_LANGUAGE_HPP

// project headers
#include "grammar.hpp"



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    const auto u_lexer_grammar = tuc::Grammar{
        {
            Rule{TokenType::LCOMMENT, "//(.*)(\\n|$)", 0},
            Rule{TokenType::ASSIGN, "\\=", 0, 14},
            Rule{TokenType::ADD, "\\+", 0, 3, Associativity::LEFT},
            Rule{TokenType::SUBTRACT, "\\-", 0, 3, Associativity::LEFT},
            Rule{TokenType::MULTIPLY, "\\*", 0, 4, Associativity::LEFT},
            Rule{TokenType::DIVIDE, "\\/", 0, 4, Associativity::LEFT},
            Rule{TokenType::INTEGER, "\\d+", 0},
            Rule{TokenType::LPAREN, "\\(", 0},
            Rule{TokenType::RPAREN, "\\)", 0},
            Rule{TokenType::SEMICOL, ";", 0},
            Rule{TokenType::IDENTIFIER, "\\b[A-Za-z_]+\\b", 0, 0, Associativity::RIGHT}
        }
    };
}

#endif//TUC_U_LANGUAGE_HPP
