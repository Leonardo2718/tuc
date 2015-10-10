/*
Project: TUC
File: compiler_exceptions.hpp
Author: Leonardo Banderali
Created: October 9, 2015
Last Modified: October 9, 2015

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

#ifndef COMPILER_EXCEPTIONS_HPP
#define COMPILER_EXCEPTIONS_HPP

// standard libraries
#include <exception>

//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    namespace CompilerException {
        class CompilerError; // a exception class for generating compilation errors
    }
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
A exception class for generating compilation errors.
*/
class tuc::CompilerException::CompilerError: public std::exception {
    public:
        CompilerError(const char* _file, const int _position) noexcept;

        const char* file() const noexcept;
        /*  returns the file where the error was found */

        int position() const noexcept;
        /*  returns the position in the file where the error was found */

        virtual const char* what() const noexcept;
        /*  returns a string describing what the error was */

    private:
        const char* srcFile;
        const int pos;
};

#endif//COMPILER_EXCEPTIONS_HPP
