/*
 * Copyright (c) 2018 Leonardo Banderali
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
use std::ops::Deref;
use std::ops::DerefMut;

use utils::*;
use fmttree;

impl<T: fmttree::Display> fmttree::Display for WithPos<T> {
    fn display_node(&self) -> String {
        format!("{:30}{:?}", self.item.display_node(), self.pos())
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        self.item.display_children(f)
    }
}

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Type {
    I32,
    Unknown
}

#[derive(Debug,Clone,PartialEq)]
pub struct WithType<T> {
    pub item: T,
    pub t: Type
}

impl<T> Deref for WithType<T> {
    type Target = T;

    fn deref(&self) -> &T {
        &self.item
    }
}

impl<T> DerefMut for WithType<T> {
    fn deref_mut(&mut self) -> &mut T {
        &mut self.item
    }
}

impl<T: fmttree::Display> fmttree::Display for WithType<T> {
    fn display_node(&self) -> String {
        format!("{} :: {:?}", self.item.display_node(), self.t)
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        self.item.display_children(f)
    }
}

pub type IndirectWithPos<T> = Box<WithPos<T>>;

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum BinaryOperator {
    Add,
    Sub,
    Mul,
    Div,
}

#[derive(Debug,Clone,PartialEq)]
pub enum Expression {
    Identifier(String),
    Literal(Const),
    BinaryOp(BinaryOperator, IndirectWithPos<WithType<Expression>>, IndirectWithPos<WithType<Expression>>),
}

impl fmttree::Display for Expression {
    fn display_node(&self) -> String {
        use self::Expression::*;
        match self {
            BinaryOp(op, _, _) => format!("{:?}", op),
            _ => format!("{:?}", self)
        }
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        use self::Expression::*;
        match self {
            BinaryOp(_, lhs, rhs) => lhs.display_sub_tree(f.clone()) + &rhs.display_sub_tree(f.clone()),
            _ => String::new()
        }
    }
}

#[derive(Debug,Clone,PartialEq)]
pub enum Statement {
    Print(WithPos<WithType<Expression>>)
}
pub type StatementList = Vec<WithPos<Statement>>;

impl fmttree::Display for Statement {
    fn display_node(&self) -> String {
        use self::Statement::*;
        match self {
            Print(_) => "Print".to_string()
        }
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        use self::Statement::*;
        match self {
            Print(e) => e.display_sub_tree(f)
        }
    }
}

#[derive(Debug,Clone,PartialEq)]
pub struct Program {
    pub body: StatementList
}

impl fmttree::Display for Program {
    fn display_node(&self) -> String {
        "program".to_string()
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        let mut s = String::new();
        for stmt in &self.body {
            s += &format!("{}", stmt.display_sub_tree(f.clone()));
        }
        s
    }
}