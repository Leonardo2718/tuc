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

#[derive(Debug,Clone,PartialEq)]
pub enum Statement {
    Print(WithPos<WithType<Expression>>)
}
pub type StatementList = Vec<WithPos<Statement>>;

#[derive(Debug,Clone,PartialEq)]
pub struct Program {
    pub body: StatementList
}