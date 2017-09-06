/*
Project: TUC
File: compiler_exceptions.hpp
Author: Leonardo Banderali
Created: October 9, 2015
Last Modified: January 5, 2016

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

#ifndef TUC_COMPILER_EXCEPTIONS_HPP
#define TUC_COMPILER_EXCEPTIONS_HPP

// project headers
#include "text_entity.hpp"



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    namespace CompilerException {
        class AbstractError;            // an abstract exception class for generating any kind of error

        class CompilationError;         // an abstract exception class for generating compilation errors
        class CompilerFault;            // an abstract exception class for generating compiler faults

        class UnknownSymbol;            // exception class for unknown symbol (undeclared symbols)
        class MismatchedParenthesis;    // exception class for mismatched parentheses

        class UnimplementedFeature;     // exception class for when using an unimplemented language feature
    }
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
an abstract exception class for generating any kind of error
*/
class tuc::CompilerException::AbstractError {
    public:
        virtual ~AbstractError() noexcept = default;

        virtual std::string message() const noexcept = 0;
        /*  returns a message associated with the error */

        virtual int error_code() const noexcept;
        /*  returns an error code that can be returned by the program (default is -1) */
};

/*
an abstract exception class for generating compilation errors.
*/
class tuc::CompilerException::CompilationError : public tuc::CompilerException::AbstractError {
    public:
        explicit CompilationError(FilePosition _position);

        std::string message() const noexcept;

        virtual std::string error() const noexcept = 0;

        std::string file() const noexcept;
        /*  returns the file where the error was found */

        unsigned int line() const noexcept;
        /*  returns the line number where the error was found */

        unsigned int column() const noexcept;
        /*  returns the column number where the error was found */

    private:
        FilePosition position;
};

/*
an abstract exception class for generating compiler faults

A fault is anything that the compiler understands but does not know how to handle or cannot accept.
An example is the use of an unimplemented feature or a feature that has been removed from the language.
*/
class tuc::CompilerException::CompilerFault : public tuc::CompilerException::AbstractError {
    public:
        CompilerFault() = default;

        std::string message() const noexcept final;

        virtual std::string title() const noexcept = 0;
        /*  returns a title for the fault */

        virtual std::string cause() const noexcept = 0;
        /*  returns the cause of the fault */
};

/*
exception class for unknown symbol (undeclared symbols)
*/
class tuc::CompilerException::UnknownSymbol : public tuc::CompilerException::CompilationError {
    public:
        UnknownSymbol(const TextEntity& _textEntiry);

        std::string error() const noexcept override;

    private:
        std::string errorMsg;
};

/*
exception class for mismatched parentheses
*/
class tuc::CompilerException::MismatchedParenthesis : public tuc::CompilerException::CompilationError {
    public:
        explicit MismatchedParenthesis(const TextEntity& _symbol);

        std::string error() const noexcept override;
};

/*
exception class for when using an unimplemented language feature
*/
class tuc::CompilerException::UnimplementedFeature : public tuc::CompilerException::CompilerFault {
    public:
        UnimplementedFeature(FilePosition _position, std::string _feature, std::string _cause);

        std::string title() const noexcept override;

        std::string cause() const noexcept override;

        std::string feature() const noexcept;

        std::string file() const noexcept;
        /*  returns the file where the error was found */

        unsigned int line() const noexcept;
        /*  returns the line number where the error was found */

        unsigned int column() const noexcept;
        /*  returns the column number where the error was found */

    private:
        FilePosition position;
        std::string featureName;
        std::string faultCause;
};

#endif//TUC_COMPILER_EXCEPTIONS_HPP
