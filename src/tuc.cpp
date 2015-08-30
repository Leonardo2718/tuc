/*
Project: TUC
File: tuc.cpp
Author: Leonardo Banderali
Created: August 7, 2015
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

// c++ standard libraries
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

// project headers
#include "lexer.hpp"

int main(int argc, char** argv) {
    if (argc == 2) {
        auto inputFile = std::ifstream{argv[1]};
        //std::stringstream ss;
        std::stringbuf sb;
        inputFile.get(sb, static_cast<char>(-1)); // read the entire file
        const auto fileText = sb.str();

        using MyLexer = tuc::Lexer<decltype(fileText.begin())>;
        auto rules = std::vector<MyLexer::RuleList>{{MyLexer::make_rule("name", "TUC", 0)}};
        auto lexer = MyLexer(fileText.cbegin(), fileText.cend(), rules);
        auto t = tuc::Token{};

        // test code (should print "name TUC" as many times as it appears in the file)
        do {
            t = lexer.next();
            std::cout << t.name() << " " << t.lexeme() << std::endl;
        } while (!t.name().empty());

        //std::cout << fileText << std::endl;
    }
}
