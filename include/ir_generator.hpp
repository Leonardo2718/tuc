/*
Project: TUC
File: ir_generator.hpp
Author: Leonardo Banderali
Created: June 25, 2016
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

#ifndef TUC_IR_GENERATOR_HPP
#define TUC_IR_GENERATOR_HPP

// project headers
#include "syntax_tree.hpp"
#include "inter_lang.hpp"

// standard libraries
#include <unordered_map>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {

class VariableFactory;

auto generate_ir(const SyntaxNode* syntax_tree) -> IntermediateRepresentation;

}

class tuc::VariableFactory {
    public:

        static constexpr auto Anonymous = "__temp__";

        static auto get_variable(const std::string& base_name = Anonymous) -> tuc::Value {
            if (variable_list.count(base_name) == 0) {
                variable_list[base_name] = 0;
            }

            auto suffix = variable_list[base_name];
            variable_list[base_name] = suffix + 1;

            return tuc::Value{tuc::Value::ValueKind::Variable, base_name + "#" + std::to_string(suffix)};
        }

    private:
        static std::unordered_map<std::string, unsigned int> variable_list;
};

#endif // TUC_IR_GENERATOR_HPP
