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

use il::*;

use std::fmt;
use std::error;
use std::result;
use std::mem;
use std::str;

#[derive(Debug,Clone,PartialEq)]
pub enum Error {
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl error::Error for Error {
    fn description(&self) -> &str {
        // use self::Error::*;
        match *self {
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        // use self::Error::*;
        match self {
            _ => None
        }
    }
}

pub type Result<T> = result::Result<T, Error>;

struct WatGenerator<'a> {
    il: &'a il::Body,
    wat: String,
    indent_level: usize,
}

impl<'a> WatGenerator<'a> {
    fn new(il: &il::Body) -> WatGenerator { WatGenerator{ il, wat: String::new(), indent_level: 0 } }

    fn indent(&mut self) { 
        if self.indent_level <= std::usize::MAX { self.indent_level += 2; }
    }

    fn unindent(&mut self) { 
        if self.indent_level > 0 { self.indent_level -= 2; }
    }

    fn putln(&mut self, line: &str) {
        self.wat.push_str(&" ".repeat(self.indent_level));
        self.wat.push_str(line);
        self.wat.push('\n');
    }

    fn from_opcodes(&mut self, _opcodes: &[il::OpCode]) {
    }

    fn from_terminator(&mut self, terminator: &il::Terminator) {
        use il::il::Terminator::*;
        match terminator {
            &Fallthrough => {},
            &Return => self.putln("return"),
            _ => panic!("Unhandled terminator {}.", terminator)
        }
    } 

    fn from_basic_blocks(&mut self, basic_blocks: &[il::BasicBlock]) {
        for bb in basic_blocks {
            self.putln(&format!("block {}", bb.id.0));
            self.indent();
            self.from_opcodes(&bb.opcodes);
            self.from_terminator(&bb.terminator);
            self.unindent();
            self.putln(&format!("end ;; block {}", bb.id.0));
        }
    }

    fn generate(&mut self) -> Result<String> {
        self.putln("(module");
        self.indent();
        self.putln("(func (export \"main\") (result i32)");
        self.indent();
        self.from_basic_blocks(&self.il.code);
        self.putln("i32.const 0 ;; final return value");
        self.unindent();
        self.putln(") ;; end of function 'main'");
        self.unindent();
        self.putln(") ;; end of module");
        let mut wat = String::new();
        mem::swap(&mut wat, &mut self.wat);
        return Ok(wat);
    }
}

pub fn generate_wat(il: &il::Body) -> Result<String> {
    return WatGenerator::new(il).generate();
}
