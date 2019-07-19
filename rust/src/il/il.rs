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

/*#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Constant {
    Bool(bool),
    I32(i32),
}*/

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Arith2 {
    Add, Sub, Mul, Div,
}

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Compare {
    LT, LE, EQ, GT, GE,
}

#[derive(Debug,Clone)]
pub enum OpCode {
    Arith2(Arith2, Value, Value, Value),
    Compare(Compare, Value, Value, Value),
    Copy(Value, Value),
    Set(Value, utils::Const)
}

#[derive(Debug,Clone)]
pub enum Terminator {
    Branch(BasicBlockIndex, Compare, Value, Value, Vec<Value>),
    Jump(BasicBlockIndex, Vec<Value>),
    Fallthrough,
    Return,
}

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub struct BasicBlockIndex(i32);

#[derive(Debug,Clone)]
pub struct BasicBlock {
    pub argVals: Vec<Value>,
    pub opcodes: Vec<OpCode>,
    pub terminator: Terminator,
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