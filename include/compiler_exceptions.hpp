/*
Project: TUC
File: compiler_exceptions.hpp
Author: Leonardo Banderali
Created: October 9, 2015
Last Modified: November 3, 2015

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

#ifndef TUC_COMPILER_EXCEPTIONS_HPP
#define TUC_COMPILER_EXCEPTIONS_HPP

// project headers
#include "text_entity.hpp"

// c++ standard libraries
#include <exception>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    namespace CompilerException {
        class AbstractError;    // an abstract exception class for generating any kind of error
        class CompilationError; // an abstract exception class for generating compilation errors
        class UnknownSymbol;    // exception class for unknown symbol (undeclared symbols)
    }
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
an abstract exception class for generating any kind of error
*/
class tuc::CompilerException::AbstractError : public std::exception {
    public:
        virtual ~AbstractError() noexcept = default;

        virtual const char* what() const noexcept = 0;
        /*  returns a string describing what the error was */

        virtual int error_code() const noexcept;
        /*  returns an error code that can be returned by the program (default is -1) */
};

/*
an abstract exception class for generating compilation errors.
*/
class tuc::CompilerException::CompilationError : public tuc::CompilerException::AbstractError {
    public:
        virtual const char* file() const noexcept = 0;
        /*  returns the file where the error was found */

        virtual int line() const noexcept = 0;
        /*  returns the line number where the error was found */

        virtual int column() const noexcept = 0;
        /*  returns the column number where the error was found */
};

/*
exception class for unknown symbol (undeclared symbols)
*/
class tuc::CompilerException::UnknownSymbol : public tuc::CompilerException::CompilationError {
    public:
        UnknownSymbol(const TextEntity& _culpritText);

        virtual const char* file() const noexcept;

        virtual int line() const noexcept;

        virtual int column() const noexcept;

        virtual const char* what() const noexcept;

    private:
        TextEntity culpritText; // text that caused syntax error
};

#endif//TUC_COMPILER_EXCEPTIONS_HPP