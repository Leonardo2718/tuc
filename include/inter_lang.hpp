/*
Project: TUC
File: inter_lang.hpp
Author: Leonardo Banderali
Created: June 18, 2016
Last Modified: June 25, 2016

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

#ifndef TUC_INTER_REP_HPP
#define TUC_INTER_REP_HPP

// project headers
#include "syntax_tree.hpp"

// standard libraries
#include <string>
#include <vector>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*####################################################################################################
### Here, we define an intermediate language to be used for analysis and later for code generation. ##
###                                                                                                 ##
### This particular intermediate language defines some basic arithmetic operations that take two    ##
### operands and a destination for the result.                                                      ##
####################################################################################################*/

namespace tuc {

/*
Class for representing values (variables and literals) in the intermediate language.
*/
class Value {
    public:
        enum class ValueKind {Variable, Literal};

        Value(ValueKind _kind, std::string _value) : val_kind{_kind}, val{_value} {}

        auto kind() const noexcept -> ValueKind { return val_kind; }

        auto value() const noexcept -> std::string { return val; }

    private:
        ValueKind val_kind;
        std::string val;
};

/*
Class for representing an three-operand operations.
*/
class Operation {
    public:
        enum class OperationType {ADD, SUBTRACT, MULTIPLY, DIVIDE, MODULUS};

        Operation(OperationType _operation, Value _dest, Value _src_one, Value _src_two);

        auto operation() -> OperationType const noexcept { return opType; }

        auto destination() -> Value const noexcept { return dest; }

        auto source_one() -> Value const noexcept { return src_one; }

        auto source_two() -> Value const noexcept { return src_two; }

    private:
        OperationType opType;
        Value dest;
        Value src_one;
        Value src_two;
};

using IntermediateRepresentation = std::vector<Operation>;

} // namespace tuc

#endif // TUC_INTER_REP_HPP
