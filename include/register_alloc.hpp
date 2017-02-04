/*
Project: TUC
File: register_alloc.hpp
Author: Leonardo Banderali
Created: August 21, 2016
Last Modified: August 21, 2016

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

#ifndef REGISTER_ALLOC_HPP
#define REGISTER_ALLOC_HPP

// project headers
#include "inter_lang.hpp"

// standard libraries
#include <string>
#include <vector>
#include <unordered_map>
#include <utility>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class RegisterMap;

    IntermediateRepresentation allocate_registers(IntermediateRepresentation const & ir);
    /*  allocate registers to variables in the intermediate language */
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class tuc::RegisterMap {
        using Variable = std::string;
        using Register = std::string;

    public:
        auto register_for(Variable const & var) -> Register;
        /*  returns register associated with variable `var` */

        auto variable_for(Register const & reg) -> Variable;
        /*  returns variable associated with register `reg` */

        auto associate(Register const & reg, Variable const & var) -> void;
        /*  associates variable `var` with register `reg` */

    private:
        std::unordered_map<Register, Variable> reg_map;
        std::unordered_map<Variable, Register> var_map;
};

#endif // REGISTER_ALLOC_HPP
