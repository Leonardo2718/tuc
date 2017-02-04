/*
Project: TUC
File: tuc.cpp
Author: Leonardo Banderali
Created: August 7, 2015
Last Modified: July 1, 2016

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

// project headers
#include "compiler_exceptions.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "ir_generator.hpp"
#include "register_alloc.hpp"
#include "asm_generator.hpp"
#include "abstract_tree.hpp"

// c++ standard libraries
#include <memory>
#include <fstream>
#include <iostream>
#include <string>
#include <tuple>
#include <list>



int main(int argc, char** argv) {
    if (argc == 3) {
        try {
            // tokenize the text from the input file
            auto tokens = tuc::lex_analyze(argv[1]);

            // generate a syntax tree
            auto syntaxTreeRoot = tuc::gen_syntax_tree(tokens);
            auto symbolTable = tuc::SymbolTable{};

            // generate and intermediate representation
            auto ir = tuc::generate_ir(syntaxTreeRoot.get());
            std::cout << "IR:\n" << ir;

            auto reg_alloc_ir = tuc::allocate_registers(ir);
            std::cout << "Register Allocated IR:\n" << reg_alloc_ir;

            // generate the asembly code
            auto outputASM = std::ostringstream{};
            outputASM << "section .text\nglobal _start\n\n_start:\n";

            //std::cout << syntaxTreeRoot.get();  // useful for debugging

            for (int i = 0, count = syntaxTreeRoot->child_count(); i < count; i++) {
                auto n = syntaxTreeRoot->child(i);
                //std::cout << n;             // useful for debugging
                outputASM << tuc::gen_expr_asm(n, symbolTable);
            }

            outputASM << "\nmov ebx, eax\nmov eax, 1\nint 80h\n";

            // print the asembly code to a file
            auto outputFile = std::ofstream{argv[2]};
            outputFile << outputASM.str();
            outputFile.close();
        }
        catch (const tuc::CompilerException::AbstractError& e) {
            std::cout << e.message();
            return e.error_code();
        }
    }
}
