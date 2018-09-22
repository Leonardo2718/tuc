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
use std::collections::HashMap;

use utils::*;

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
pub enum Operator {
    ADD,
    SUB,
    MUL,
    DIV,
}

macro_rules! define_keywords {
    ( $($e:expr => $t:ident),+ ) => {
        #[derive(Debug,Clone,Copy,PartialEq,Eq)]
        pub enum Keyword {
            $($t),+
        }
        
        pub fn as_keyword(k: &str) -> Option<Keyword> {
            let keywords: HashMap<&str, Keyword> = [
                $(($e, Keyword::$t)),+
                ].iter().cloned().collect();
            keywords.get(k).map(|w| *w)
        }
    };
}

define_keywords!(
    "print" => PRINT,
    "let" => LET
);

#[derive(Debug,Clone,PartialEq)]
pub enum TokenType {
    COMMENT(String),
    IDENT(String),
    CONST(Const),
    KEYWORD(Keyword),
    OPERATOR(Operator),
    LPAREN,
    RPAREN,
    SEMICOLON,
}

#[derive(Debug,Clone)]
pub struct Token {
    pub token: TokenType,
    pub pos: Position,
}
