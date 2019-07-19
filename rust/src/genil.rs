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

use std::fmt;
use std::error;
use std::result;

struct IlGenerator {
    values: value::ValueTable
}

#[derive(Debug,Clone,PartialEq)]
pub enum Error {
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
            BadAST => "AST is malformed.",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        use self::Error::*;
        match *self {
            // StatementParserError(ref e) => Some(e),
            // LexerError(ref e) => Some(e),
            _ => None
        }
    }
}

pub type Result<T> = result::Result<T, Error>;

impl IlGenerator {
    fn new() -> IlGenerator { IlGenerator{values: value::ValueTable::new()} }

    fn from_expression(&mut self, expr: &ast::Expression) -> Result<(Vec<il::BasicBlock>, value::Value)> {
        use ast::BareExpression::*;
        match expr.unwrap_pos().unwrap_type() {
            Literal(c) => {
                let v = self.values.new_value();
                let op = il::OpCode::Set(v, *c);
                let bb = il::BasicBlock{argVals: Vec::new(), opcodes: vec![op], terminator: il::Terminator::Fallthrough};
                return Ok((vec![bb], v));
            }
            BinaryOp(op, lhs, rhs) => {
                let (mut lops, l) = self.from_expression(lhs)?;
                let (mut rops, r) = self.from_expression(rhs)?;
                let mut bbs: Vec<il::BasicBlock> = Vec::new();
                bbs.append(&mut lops);
                bbs.append(&mut rops);
                bbs.push(il::BasicBlock{argVals: Vec::new(), opcodes: vec![], terminator: il::Terminator::Fallthrough});
                return Ok((Vec::new(), self.values.new_value()));
            },
            _ => Err(Error::BadAST)
        }
    }

    fn from_statment(&mut self, statement: &ast::Statement) -> Result<Vec<il::BasicBlock>> {
        use ast::BareStatement::*;
        match statement.unwrap_pos() {
            Let(_var, expr) => self.from_expression(expr).map(|x| x.0),
            Assignment(_var, expr) => self.from_expression(expr).map(|x| x.0),
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
    return Ok(il::Body{code: code, values: il_generator.values});
}