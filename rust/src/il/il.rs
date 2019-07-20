/*
 * Copyright (c) 2019 Leonardo Banderali
 *
 * This software is released under the MIT License:
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

use utils;
use il::value;
use il::value::Value;
use std::fmt;

/*#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Constant {
    Bool(bool),
    I32(i32),
}*/

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Arith2 {
    Add, Sub, Mul, Div,
}

impl fmt::Display for Arith2 {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Compare {
    LT, LE, EQ, GT, GE,
}

impl fmt::Display for Compare {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

#[derive(Debug,Clone)]
pub enum OpCode {
    Arith2(Arith2, Value, Value, Value),
    Compare(Compare, Value, Value, Value),
    Copy(Value, Value),
    Set(Value, utils::Const)
}

impl fmt::Display for OpCode {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use self::OpCode::*;
        match self {
            &Arith2(op, res, lhs, rhs) => write!(f, "    {} {} {} {}", op, res, lhs, rhs),
            &Compare(cmp, res, lhs, rhs) => write!(f, "    {} {} {} {}", cmp, res, lhs, rhs),
            &Copy(dest, src) => write!(f, "    Copy {} {}", dest, src),
            &Set(v, c) => write!(f, "    Set {} {}", v, c),
            _ => write!(f, "    {:?}", self)
        }
    }
}

#[derive(Debug,Clone)]
pub enum Terminator {
    Branch(BasicBlockIndex, Compare, Value, Value, Vec<Value>),
    Jump(BasicBlockIndex, Vec<Value>),
    Fallthrough,
    Return,
}

impl fmt::Display for Terminator {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use self::Terminator::*;
        match self {
            Jump(BasicBlockIndex(i), vals) => write!(f, "    Jump {} ({:?})", i, vals),
            _ => write!(f, "    {:?}", self)
        }
    }
}

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub struct BasicBlockIndex(i32);

#[derive(Debug,Clone)]
pub struct BasicBlock {
    pub argVals: Vec<Value>,
    pub opcodes: Vec<OpCode>,
    pub terminator: Terminator,
}

impl fmt::Display for BasicBlock {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let mut opcodes = String::new();
        for opcode in &self.opcodes {
            opcodes.push_str(&format!("{}\n", opcode));
        }
        write!(f, "BasicBlock ({:?}):\n{}{}\n", self.argVals, opcodes, self.terminator)
    }
}

#[derive(Debug,Clone)]
pub struct Body {
    pub code: Vec<BasicBlock>,
    pub values: value::ValueTable,
}

impl Body {
    pub fn new() -> Body {
        Body{code:Vec::new(), values:value::ValueTable::new()}
    }
}

impl fmt::Display for Body {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        let mut s = String::from("IL:\n");
        for bb in &self.code {
            s.push_str(&format!("{}", bb));
        }
        write!(f, "{}Values:\n{:?}\n", s, self.values)
    }
}