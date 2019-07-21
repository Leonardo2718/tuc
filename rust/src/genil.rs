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
use ast;
use il::*;
use symtab;

use std::fmt;
use std::error;
use std::result;
use std::convert;

#[derive(Debug,Clone,PartialEq)]
pub enum Error {
    BadSymbolUse(symtab::Error),
    BadAST
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl error::Error for Error {
    fn description(&self) -> &str {
        use self::Error::*;
        match *self {
            BadSymbolUse(_) => "Improper use of a symbol",
            BadAST => "AST is malformed.",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        use self::Error::*;
        match self {
            &BadSymbolUse(ref e) => Some(e),
            _ => None
        }
    }
}

pub type Result<T> = result::Result<T, Error>;

impl convert::From<symtab::Error> for Error {
    fn from(error: symtab::Error) -> Error {
        Error::BadSymbolUse(error)
    }
}

struct IlGenerator {
    values: value::ValueTable,
    symbol_table: symtab::SymbolTable,
    block_counter: i32,
}

impl IlGenerator {
    fn new() -> IlGenerator { 
        IlGenerator{values: value::ValueTable::new(), symbol_table: symtab::SymbolTable::new(), block_counter: 0} 
    }

    fn new_block(&mut self, argVals: Vec<value::Value>, opcodes: Vec<il::OpCode>, terminator: il::Terminator) -> il::BasicBlock {
        let id = self.block_counter;
        self.block_counter += 1;
        return il::BasicBlock{id: il::BasicBlockId(id), argVals, opcodes, terminator, nextVals: vec![]};
    }

    fn to_il_op(&self, op: ast::BinaryOperator) -> il::Arith2 {
        use ast::BinaryOperator as bop;
        use il::il::Arith2;
        match op {
            bop::Add => Arith2::Add,
            bop::Sub => Arith2::Sub,
            bop::Mul => Arith2::Mul,
            bop::Div => Arith2::Div,
        }
    }

    fn from_expression(&mut self, expr: &ast::Expression) -> Result<(Vec<il::OpCode>, value::Value)> {
        use ast::BareExpression::*;
        match expr.unwrap_pos().unwrap_type() {
            Literal(c) => {
                let v = self.values.new_value();
                let op = il::OpCode::Set(v, *c);
                return Ok((vec![op], v));
            }
            BinaryOp(op, lhs, rhs) => {
                let (mut lops, l) = self.from_expression(lhs)?;
                let (mut rops, r) = self.from_expression(rhs)?;
                let mut ops: Vec<il::OpCode> = Vec::new();
                ops.append(&mut lops);
                ops.append(&mut rops);
                let val = self.values.new_value();
                ops.append(&mut vec![il::OpCode::Arith2(self.to_il_op(*op), val, l, r)]);
                return Ok((ops, val));
            },
            _ => Err(Error::BadAST)
        }
    }

    fn from_statment(&mut self, statement: &ast::Statement) -> Result<Vec<il::BasicBlock>> {
        use ast::BareStatement::*;
        match statement.unwrap_pos() {
            Let(var, expr) => {
                let ty = expr.get_type();
                let (ops, val) = self.from_expression(expr)?;
                self.symbol_table.define_symbol(var.to_string(), ty)?;
                self.symbol_table.set_value(var, val)?;
                return Ok(vec![self.new_block(vec![], ops, il::Terminator::Fallthrough)]);
                },
            Assignment(var, expr) => {
                let (ops, val) = self.from_expression(expr)?;
                self.symbol_table.set_value(var, val)?;
                return Ok(vec![self.new_block(vec![], ops, il::Terminator::Fallthrough)]);
                },
            _ => Err(Error::BadAST)
        }
    }

    fn from_statment_list(&mut self, statements: &ast::StatementList) -> Result<Vec<il::BasicBlock>> {
        //return statements.iter().map(|s| self.from_statment(&s)).flatten().collect();
        let mut blocks: Vec<il::BasicBlock> = Vec::new();
        for s in statements {
            blocks.append(&mut self.from_statment(s)?);
        }
        return Ok(blocks);
    }
}

pub fn gen_il(program: &ast::Program) -> Result<il::Body> {
    let mut il_generator = IlGenerator::new();
    let code = il_generator.from_statment_list(&program.body)?;
    return Ok(il::Body{code, values: il_generator.values});
}