/*
Project: TUC
File: symbol_table.hpp
Author: Leonardo Banderali
Created: October 8, 2015
Last Modified: October 8, 2015

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

#ifndef SYMBOL_TABLE_HPP
#define SYMBOL_TABLE_HPP

// standard libraries
#include <unordered_map>
#include <string>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class Symbol;   // a class for representing symbols in the source code
    using SymbolTable = std::unordered_map<std::string, Symbol>;
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
A class for representing symbols in the source code. Currently, there are two kinds of symbols: FUNCTIONs and
PROCEDUREs. A FUNCTION is for declarative funtions like those used in Haskell. These cannot have side effects. So, a
FUNCTION with no parameters is just a plain variable. A PROCEDURE is just an emperative function, like C style functions.
*/
class tuc::Symbol {
    public:
        enum class SymbolType {FUNCTION, PROCEDURE};

        Symbol(SymbolType _type, std::string _value, int _argCount);

        SymbolType type() const noexcept;

        std::string value() const noexcept;

        int arg_count() const noexcept;
        /*  returns the number of arguments the entity can take */

    private:
        SymbolType symbolType;
        std::string symbolValue;
        int argumentCount;
};

#endif//SYMBOL_TABLE_HPP
