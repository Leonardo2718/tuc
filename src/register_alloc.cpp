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

// project headers
#include "register_alloc.hpp"

// standard libraries
#include <iostream>
#include <deque>
#include <algorithm>

/*
returns register associated with variable `var`
*/
auto tuc::RegisterMap::register_for(Variable const & var) -> Register {
    return var_map[var];
}

/*
returns variable associated with register `reg`
*/
auto tuc::RegisterMap::variable_for(Register const & reg) -> Variable {
    return reg_map[reg];
}

/*
associates variable `var` with register `reg`
*/
auto tuc::RegisterMap::associate(Register const & reg, Variable const & var) -> void {
    reg_map[reg] = var;
    var_map[var] = reg;
}

/*
allocate registers to variables in the intermediate language
*/
tuc::IntermediateRepresentation tuc::allocate_registers(IntermediateRepresentation const & ir) {
    auto reg_allocated_ir = IntermediateRepresentation{};
    reg_allocated_ir.reserve(ir.size());

    auto reg_set = std::vector<std::string>{"eax", "ebx", "ecx", "edx"};

    auto var_stack = std::vector<std::string>{};

    auto map = RegisterMap{};

    auto get_unallocated_reg = [&](){
        for (auto const& reg : reg_set) {
            if (map.variable_for(reg) == "") {
                return reg;
            }
        }

        return std::string{};
    };

    auto try_allocate_reg = [&](std::string const & val){
        auto reg = map.register_for(val);
        if (reg == "") {
            reg = get_unallocated_reg();
            if (reg != "") {
                map.associate(reg, val);
            }
        }
        return reg;
    };

    auto get_register_to_leak = [](auto&& first_op, auto&& last_op, auto&& reg_map) {
        auto reg_set = std::vector<std::string>{"eax", "ebx", "ecx", "edx"};
        auto start = reg_set.begin();
        auto end = reg_set.end();
        for (auto op = first_op; op != last_op; ++op) {
            if (reg_set.size() == 1) {
                break;
            }
            else {
                end = std::remove(start, end, reg_map.register_for(op->source_one().value()));
            }

            if (reg_set.size() == 1) {
                break;
            }
            else {
                end = std::remove(start, end, reg_map.register_for(op->source_two().value()));
            }
        }
        return std::make_pair(reg_map.register_for(reg_set[0]), reg_set[0]);
    };

    for (auto in_op = ir.cbegin(), last_op = ir.cend(); in_op != last_op; ++in_op) {
    //for (int i = 0; i < ir.size(); i++) {
    //for (auto in_op : ir) {
        //auto in_op = ir[i];
        using ValKind = Value::ValueKind;
        using OpType = Operation::OperationType;

        auto op = in_op->operation();
        auto src1 = in_op->source_one();
        auto src2 = in_op->source_two();

        if (src1.kind() == ValKind::Variable) {
            auto reg = try_allocate_reg(src1.value());
            if (reg == "") {
                auto leak = get_register_to_leak(in_op+1, last_op, map);

                reg_allocated_ir.push_back(Operation{OpType::SAVE, src1, Value{}, Value{}});
                var_stack.push_back(src1.value());

                reg = std::get<0>(leak);
                map.associate(reg, src1.value());
            }
            src1 = Value{ValKind::Variable, reg};
        }

        reg_allocated_ir.push_back(Operation{op, Value{ValKind::Variable, "eax"}, src1, src2});
    }

    return reg_allocated_ir;
}
