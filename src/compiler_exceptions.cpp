/*
Project: TUC
File: compiler_exceptions.cpp
Author: Leonardo Banderali
Created: October 9, 2015
Last Modified: November 8, 2015

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

// project headers
#include "compiler_exceptions.hpp"

// c++ standard libraries
#include <sstream>
#include <iostream>


//~class implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
returns an error code that can be returned by the program (default is -1)
*/
int tuc::CompilerException::AbstractError::error_code() const noexcept {
    return -1;
}



tuc::CompilerException::CompilationError::CompilationError(FilePosition _position) : position{_position} {}

std::string tuc::CompilerException::CompilationError::message() const noexcept {
    std::stringstream text;
    text << "Error: " << what() << " in file `" << file() << "` at line " << line() << ", column " << column() << ".\n";
    return text.str();
}

/*
returns the file where the error was found
*/
const char* tuc::CompilerException::CompilationError::file() const noexcept {
    return position.file_path().c_str();
}

/*
returns the line number where the error was found
*/
unsigned int tuc::CompilerException::CompilationError::line() const noexcept {
    return position.line();
}

/*
returns the column number where the error was found
*/
unsigned int tuc::CompilerException::CompilationError::column() const noexcept {
    return position.column();
}



tuc::CompilerException::UnknownSymbol::UnknownSymbol(const TextEntity& _symbol) : CompilationError{_symbol.position()} {
    std::stringstream text;
    text << "Unknown symbol `" << _symbol.text() << "`";
    error = text.str();
}

const char* tuc::CompilerException::UnknownSymbol::what() const noexcept {
    return error.c_str();
}



tuc::CompilerException::MismatchedParenthesis::MismatchedParenthesis(const TextEntity& _symbol) : CompilationError{_symbol.position()} {}

const char* tuc::CompilerException::MismatchedParenthesis::what() const noexcept {
    return "Mismatched parenthesis";
}
