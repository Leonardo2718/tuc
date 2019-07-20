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

use ast;
use il::*;

use std::fmt;
use std::error;
use std::result;
use std::collections::HashMap;

#[derive(Debug,Clone,PartialEq)]
pub enum Error {
    UndefinedSymbol(String),
    NoScopeAvailable,
    DroppingLastScope,
}

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl error::Error for Error {
    fn description(&self) -> &str {
        use self::Error::*;
        match self {
            UndefinedSymbol(_) => "Use of undefined symbol",
            NoScopeAvailable => "Internal error: no scope available for symbol operations",
            DroppingLastScope => "Internal error: removing last available scope",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        None
    }
}

pub type Result<T> = result::Result<T, Error>;

struct SymbolTableEntry {
    ty: ast::Type,
    value: Option<value::Value>,
}

type Scope = HashMap<String, SymbolTableEntry>;

pub struct SymbolTable {
    table: Vec<Scope>,
}

impl SymbolTable {
    pub fn new() -> SymbolTable { SymbolTable{table: vec![HashMap::new()]} }

    fn top_scope(&self) -> Result<&Scope> {
        let len = self.table.len();
        if len == 0 {
            Err(Error::NoScopeAvailable)
        } else {
            Ok(&self.table[len - 1])
        }
    }

    fn mut_top_scope(&mut self) -> Result<&mut Scope> {
        let len = self.table.len();
        if len == 0 {
            Err(Error::NoScopeAvailable)
        } else {
            Ok(&mut self.table[len - 1])
        }
    }

    fn get_entry(&self, symbol: &str) -> Option<&SymbolTableEntry> {
        for scope in self.table.iter().rev() {
            if let Some(entry) = scope.get(symbol) {
                return Some(entry);
            }
        }
        return None;
    }

    fn get_mut_entry(&mut self, symbol: &str) -> Option<&mut SymbolTableEntry> {
        for scope in self.table.iter_mut().rev() {
            if let Some(entry) = scope.get_mut(symbol) {
                return Some(entry);
            }
        }
        return None;
    }

    pub fn push_scope(&mut self) {
        self.table.push(HashMap::new());
    }

    pub fn pop_scope(&mut self) -> Result<()> {
        if self.table.len() == 1 {
            Err(Error::DroppingLastScope)
        } else {
            self.table.pop();
            Ok(())
        }
    }

    pub fn define_symbol(&mut self, symbol: String, ty: ast::Type) -> Result<()> {
        self.mut_top_scope()?.insert(symbol, SymbolTableEntry{ty, value: None});
        return Ok(());
    }

    pub fn is_defined(&self, symbol: &str) -> bool {
        self.get_entry(symbol).is_some()
    }

    pub fn is_in_top_scope(&self, symbol: &str) -> Result<bool> {
        self.top_scope().map(|t| t.contains_key(symbol))
    }

    pub fn set_value(&mut self, symbol: &str, value: value::Value) -> Result<()> {
        self.get_mut_entry(symbol).ok_or(Error::UndefinedSymbol(symbol.to_string()))?.value = Some(value);
        return Ok(());
    }

    pub fn get_value(&self, symbol: &str) -> Result<Option<value::Value>> {
        return self.get_entry(symbol).ok_or(Error::UndefinedSymbol(symbol.to_string())).map(|s| s.value);
    }

    pub fn get_type(&self, symbol: &str) -> Result<ast::Type> {
        return self.get_entry(symbol).ok_or(Error::UndefinedSymbol(symbol.to_string())).map(|s| s.ty);
    }
}