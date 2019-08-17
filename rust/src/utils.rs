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

use types;

use std::ops::Deref;
use std::ops::DerefMut;
use std::fmt;

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub struct Position {
    pub pos: usize,
}

#[derive(Debug,Clone,PartialEq)]
pub struct WithPos<T> {
    pub item: T,
    pub position: Position
}

impl<T> WithPos<T> {
    pub fn pos(&self) -> Position {
        self.position
    }

    pub fn unwrap_pos(&self) -> &T {
        &self.item
    }
}

impl<T> Deref for WithPos<T> {
    type Target = T;

    fn deref(&self) -> &T {
        &self.item
    }
}

impl<T> DerefMut for WithPos<T> {
    fn deref_mut(&mut self) -> &mut T {
        &mut self.item
    }
}

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Const {
    I32(i32),
}

impl Const {
    pub fn get_type(&self) -> types::Type {
        use types::{Type::*, Arithmetic::*, Integer::*, Float::*, Bool::*};
        match self {
            &Const::I32(_) => Arithmetic(Integer(I32))
        }
    }
}

impl fmt::Display for Const {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

/**
 * Helper function for printing iterable values as a comma separated list.
 */
pub fn format_list<T> (mut iter: T) -> String where 
    T: Iterator,
    <T as Iterator>::Item: fmt::Display 
    {
    let first =  iter.next().map(|v| v.to_string()).unwrap_or("".to_string());
    iter.map(|v| format!(", {}", v)).fold(first, |acc, s| acc + &s)
}