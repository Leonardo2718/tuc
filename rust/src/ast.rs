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

impl fmttree::Display for String {
    fn display_node(&self) -> String{
        self.clone()
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        "".to_string()
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

pub trait Passthrough<T>: Deref + DerefMut {
    fn bare(x: T) -> Self;
}

impl<T> Passthrough<T> for WithType<T> {
    fn bare(x: T) -> WithType<T> { WithType{item: x, t: Type::Unknown} }
}

impl<T> Passthrough<T> for WithPos<T> {
    fn bare(x: T) -> WithPos<T> { WithPos{item: x, position: Position{pos:0}} }
}

impl<T> Passthrough<T> for WithPos<WithType<T>> {
    fn bare(x: T) -> WithPos<WithType<T>> { WithPos{item: WithType{item: x, t: Type::Unknown} , position: Position{pos:0}} }
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

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum BinaryOperator {
    Add,
    Sub,
    Mul,
    Div,
}

#[derive(Debug,Clone,PartialEq)]
pub enum BareExpression {
    Identifier(String),
    Literal(Const),
    BinaryOp(BinaryOperator, Box<Expression>, Box<Expression>),
}

impl fmttree::Display for BareExpression {
    fn display_node(&self) -> String {
        use self::BareExpression::*;
        match self {
            BinaryOp(op, _, _) => format!("{:?}", op),
            _ => format!("{:?}", self)
        }
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        use self::BareExpression::*;
        match self {
            BinaryOp(_, lhs, rhs) => lhs.display_sub_tree(f.clone()) + &rhs.display_sub_tree(f.clone()),
            _ => String::new()
        }
    }
}

pub type Expression = WithPos<WithType<BareExpression>>;

#[derive(Debug,Clone,PartialEq)]
pub struct BareBlock(pub StatementList);

impl fmttree::Display for BareBlock {
    fn display_node(&self) -> String {
        "Block".to_string()
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        self.0.iter().map(|s| s.display_sub_tree(f.clone())).fold(String::new(), |acc, x| acc + &x)
    }
}

pub type Block = WithPos<BareBlock>;

#[derive(Debug,Clone,PartialEq)]
pub struct IfStatement {
    pub expr: Expression,
    pub body: Block,
    pub elseifs: Vec<ElseIfStatement>,
    pub elseBlock: Option<ElseStatement>,
}

impl fmttree::Display for IfStatement {
    fn display_node(&self) -> String {
        "If".to_string()
    }

    fn display_children(&self, f:fmttree::TreeFormat) -> String {
        let mut s = String::new();
        s += &self.expr.display_sub_tree(f.clone());
        s += &self.body.display_sub_tree(f.clone());
        s = self.elseifs.iter().map(|s| s.display_sub_tree(f.clone())).fold(s, |acc, x| acc + &x);
        if let Some(ref b) = self.elseBlock {
            s += &b.display_sub_tree(f.clone());
        }
        return s;
    }
}

#[derive(Debug,Clone,PartialEq)]
pub struct BareElseIfStatement {
    pub expr: Expression,
    pub body: Block,
}

impl fmttree::Display for BareElseIfStatement {
    fn display_node(&self) -> String {
        "ElseIf".to_string()
    }

    fn display_children(&self, f:fmttree::TreeFormat) -> String {
        let mut s = String::new();
        s += &self.expr.display_sub_tree(f.clone());
        s += &self.body.display_sub_tree(f.clone());
        return s;
    }
}

type ElseIfStatement = WithPos<BareElseIfStatement>;

#[derive(Debug,Clone,PartialEq)]
pub struct BareElseStatement {
    pub body: Block,
}

impl fmttree::Display for BareElseStatement {
    fn display_node(&self) -> String {
        "Else".to_string()
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        self.body.display_sub_tree(f.clone())
    }
}

type ElseStatement = WithPos<BareElseStatement>;

#[derive(Debug,Clone,PartialEq)]
pub struct WhileLoop {
    pub expr: Expression,
    pub body: Block,
}

impl fmttree::Display for WhileLoop {
    fn display_node(&self) -> String {
        "While".to_string()
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        let mut s = String::new();
        s += &self.expr.display_sub_tree(f.clone());
        s += &self.body.display_sub_tree(f.clone());
        return s;
    }
}

#[derive(Debug,Clone,PartialEq)]
pub enum BareStatement {
    Print(Expression),
    Let(WithPos<String>, Expression),
    If(IfStatement),
    While(WhileLoop),
    Assignment(WithPos<WithType<String>>, Expression),
}

pub type Statement = WithPos<BareStatement>;
pub type StatementList = Vec<Statement>;

impl fmttree::Display for BareStatement {
    fn display_node(&self) -> String {
        use self::BareStatement::*;
        match self {
            Print(_) => "Print".to_string(),
            Let(_,_) => "Let".to_string(),
            If(s) => s.display_node(),
            While(s) => s.display_node(),
            Assignment(_,_) => "Assignment".to_string(),
        }
    }

    fn display_children(&self, f: fmttree::TreeFormat) -> String {
        use self::BareStatement::*;
        match self {
            Print(e) => e.display_sub_tree(f),
            Let(i,e) => i.display_sub_tree(f.clone()) + &e.display_sub_tree(f),
            If(s) => s.display_children(f.clone()),
            While(s) => s.display_children(f.clone()),
            Assignment(i, e) => i.display_sub_tree(f.clone()) + &e.display_sub_tree(f),
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
