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
use std::collections::HashMap;

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


struct SymbolTableEntry {
    ty: ast::Type,
    value: Option<value::Value>,
}

struct SymbolTable {
    table: HashMap<String, SymbolTableEntry>,
}

impl SymbolTable {
    fn new() -> SymbolTable { SymbolTable{table: HashMap::new()} }

    fn define_symbol(&mut self, symbol: String, ty: ast::Type) {
        self.table.insert(symbol, SymbolTableEntry{ty, value: None});
    }

    fn set_value(&mut self, symbol: &str, value: value::Value) -> Result<()> {
        self.table.get_mut(symbol).ok_or(Error::BadAST)?.value = Some(value);
        return Ok(());
    }

    fn get_value(&self, symbol: &str) -> Result<Option<value::Value>> {
        return self.table.get(symbol).ok_or(Error::BadAST).map(|s| s.value);
    }

    fn get_type(&self, symbol: &str) -> Result<ast::Type> {
        return self.table.get(symbol).ok_or(Error::BadAST).map(|s| s.ty);
    }
}

struct IlGenerator {
    values: value::ValueTable,
    symbolTable: SymbolTable,
}

impl IlGenerator {
    fn new() -> IlGenerator { 
        IlGenerator{values: value::ValueTable::new(), symbolTable: SymbolTable::new()} 
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
                let val = self.values.new_value();
                let ops = vec![il::OpCode::Arith2(self.to_il_op(*op), val, l, r)];
                bbs.push(il::BasicBlock{argVals: Vec::new(), opcodes: ops, terminator: il::Terminator::Fallthrough});
                return Ok((bbs, val));
            },
            _ => Err(Error::BadAST)
        }
    }

    fn from_statment(&mut self, statement: &ast::Statement) -> Result<Vec<il::BasicBlock>> {
        use ast::BareStatement::*;
        match statement.unwrap_pos() {
            Let(var, expr) => {
                let ty = expr.get_type();
                let (bbs, val) = self.from_expression(expr)?;
                self.symbolTable.define_symbol(var.to_string(), ty);
                self.symbolTable.set_value(var, val)?;
                return Ok(bbs);
                },
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
    return Ok(il::Body{code, values: il_generator.values});
}