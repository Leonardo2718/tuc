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
use types;
use ast;
use symtab;

use std::fmt;
use std::error;
use std::result;

#[derive(Debug,Clone,PartialEq)]
pub enum Error {
    OperandTypeMissmatch(ast::BinaryOperator, types::Type, types::Type, utils::Position),
    AssignmentTypeMissmatch(String, types::Type, types::Type, utils::Position),
    SymbolError(symtab::Error, utils::Position),
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        use self::Error::*;
        match *self {
            OperandTypeMissmatch(op, lt, rt, p) => write!(f, "'{:?}' (at {}) operand type missmatch: ({:?}, {:?}).", op, p.pos, lt, rt),
            AssignmentTypeMissmatch(ref s, st, et, p) => write!(f, "Symbol '{}' of type {:?} assigned value of incorrect type {:?} at {:?}", s, st, et, p),
            SymbolError(_, p) => write!(f, "Incorrect use of symbol at {}.", p.pos),
        }
    }
}

impl error::Error for Error {
    fn description(&self) -> &str {
        use self::Error::*;
        match *self {
            OperandTypeMissmatch(_, _, _, _) => "Operand types for binary operator don't match",
            AssignmentTypeMissmatch(_, _, _, _) => "Symbol assigned value of incorrect type",
            SymbolError(_, _) => "Incorrect use of symbol",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        use self::Error::*;
        match *self {
            SymbolError(ref e, _) => Some(e),
            _ => None
        }
    }
}

pub type Result<T> = result::Result<T, Error>;

struct TypeAnalyzer {
    symbol_table: symtab::SymbolTable,
}

impl TypeAnalyzer {
    fn new() -> TypeAnalyzer { TypeAnalyzer{symbol_table: symtab::SymbolTable::new()} }

    fn type_of_expr(&mut self, expr: &mut ast::Expression) -> Result<types::Type> {
        let ty = match expr.item.item {
            ast::BareExpression::Identifier(ref s) => self.symbol_table.symbol_type(&s).map_err(|e| Error::SymbolError(e, expr.pos()))?,
            ast::BareExpression::Literal(c) => c.get_type(),
            ast::BareExpression::BinaryOp(op, ref mut lhs, ref mut rhs) => {
                let lhs_ty = self.type_of_expr(lhs)?;
                let rhs_ty = self.type_of_expr(rhs)?;
                if lhs_ty != rhs_ty { return Err(Error::OperandTypeMissmatch(op, lhs_ty, rhs_ty, expr.position)) };
                lhs_ty
            },
        };
        expr.item.t = ast::Type::from(ty);
        return Ok(ty);
    }

    fn types_in_statement(&mut self, statement: &mut ast::Statement) -> Result<()> {
        let pos = statement.pos();
        match &mut statement.item {
            &mut ast::BareStatement::Let(ref sym, ref mut expr) => {
                let ty = self.type_of_expr(expr)?;
                self.symbol_table.define_symbol(sym.unwrap_pos().clone(), ty).map_err(|e| Error::SymbolError(e, pos))?;
                return Ok(());
            },
            &mut ast::BareStatement::Assignment(ref mut sym, ref mut expr) => {
                let sym_ty = self.symbol_table.symbol_type(sym).map_err(|e| Error::SymbolError(e, sym.pos()))?;
                sym.t = ast::Type::from(sym_ty);
                let expr_ty = self.type_of_expr(expr)?;
                if sym_ty == expr_ty { Ok(()) }
                else { Err(Error::AssignmentTypeMissmatch(sym.unwrap_pos().unwrap_type().clone(), sym_ty, expr_ty, sym.pos())) }
            },
            s => panic!("Unhandled statement {:?}", s),
        }
    }

    fn analyze_types(&mut self, ast: &mut ast::Program) -> Result<()> {
        for statement in ast.body.iter_mut() {
            self.types_in_statement(statement)?;
        }
        Ok(())
    }
}

pub fn analyze_semantics(ast: &mut ast::Program) -> Result<()> {
    let mut analyzer = TypeAnalyzer::new();
    analyzer.analyze_types(ast)?;
    Ok(())
}