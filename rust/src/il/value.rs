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

use std::fmt;

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Type {
    Bool,
    I32,
}

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub struct Value {
    id: usize,
}

impl fmt::Display for Value {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "%{}", self.id)
    }
}

#[derive(Debug,Clone)]
pub struct ValueTable {
    values: Vec<Value>,
}

impl ValueTable {
    pub fn new() -> ValueTable {
        ValueTable{values:Vec::new()}
    }

    pub fn new_value(&mut self) -> Value {
        let v = Value{id:self.values.len()};
        self.values.push(v);
        return v;
    }

    pub fn new_values(&mut self, n: usize) -> Vec<Value> {
        let mut ret: Vec<Value> = Vec::new();
        for _ in 0..n {
            ret.push(self.new_value());
        }
        return ret;
    }
}
