/*
Project: TUC
File: inter_lang.cpp
Author: Leonardo Banderali
Created: June 19, 2016
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
#include "inter_lang.hpp"



//~function implementation~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::ostream& operator << (std::ostream& os, const tuc::Value& val) {
    if (val.kind() == tuc::Value::ValueKind::Variable) {
        os << "{" << val.value() << "}";
    }
    else {
        os << val.value();
    }

    return os;
}

std::ostream& operator << (std::ostream& os, tuc::Operation::OperationType op_type) {
    using OpType = tuc::Operation::OperationType;

    switch (op_type) {
        case OpType::ADD: os << "ADD"; break;
        case OpType::SUB: os << "SUB"; break;
        case OpType::MUL: os << "MUL"; break;
        case OpType::DIV: os << "DIV"; break;
        case OpType::MOD: os << "MOD"; break;
    }

    return os;
}

std::ostream& operator << (std::ostream& os, const tuc::Operation& op) {
    os << op.operation() << " "
       << op.destination() << " "
       << op.source_one() << " "
       << op.source_two();

    return os;
}

std::ostream& operator << (std::ostream& os, const tuc::IntermediateRepresentation& ir) {
    for (auto&& op : ir) {
        os << op << "\n";
    }

    return os;
}
