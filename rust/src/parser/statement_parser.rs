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
use std::error;
use std::result;
use std::convert;
use std::iter;
use std::ops;

use super::expression_parser;
use super::expression_parser::parse_expression;

use utils::*;
use token;
use lexer;
use lexer::Lexer;
use lexer::TokenIterator;
use ast;

#[derive(Debug,Clone,PartialEq)]
pub enum ParseError {
    UnexpectedToken(token::TokenType),
    ExpectingMoreTokens,
    ExpressionParserError(expression_parser::ParserError),
    LexerError(lexer::Error),
}

impl fmt::Display for ParseError {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl error::Error for ParseError {
    fn description(&self) -> &str {
        use self::ParseError::*;
        match *self {
            UnexpectedToken(_) => "Encountered unexpected token while parsing.",
            ExpectingMoreTokens => "Expected to find more tokens but got nothing",
            ExpressionParserError(_) => "Error occurred in expression parser",
            LexerError(_) => "Error occurred during lexing.",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        use self::ParseError::*;
        match *self {
            ExpressionParserError(ref e) => Some(e),
            LexerError(ref e) => Some(e),
            _ => None
        }
    }
}

pub type Error = WithPos<ParseError>;

impl convert::From<lexer::Error> for Error {
    fn from(error: lexer::Error) -> Error {
        Error{item: ParseError::LexerError(error.clone()), position: error.position}
    }
}

impl convert::From<expression_parser::Error> for Error {
    fn from(error: expression_parser::Error) -> Error {
        Error{item: ParseError::ExpressionParserError(error.item), position: error.position}
    }
}

pub type Result<T> = result::Result<T, Error>;

macro_rules! expect_token {
    ($pos:expr, $lexer:expr, $($p:pat => $e:expr),+ ) => {
        if let Some(next) = $lexer.next() {
            let next = next?;
            match next.token {
                $( $p => Ok(($e, next.pos)) ),+,
                t => Err(WithPos{item: ParseError::UnexpectedToken(t), position: next.pos})
            }
        }
        else {
            Err(WithPos{item: ParseError::ExpectingMoreTokens, position: $pos})
        }
    };
}

pub fn parse_statement<L: Lexer>(lexer: &mut L) -> Result<WithPos<ast::Statement>>  {
    use token::TokenType::*;
    use token::Keyword::*;
    use token::Operator::*;
    use ast::Statement::*;

    let token = lexer.next().unwrap()?;
    match token.token {
        KEYWORD(PRINT) => {
            expect_token!(token.pos, lexer, LPAREN => ())?;
            let expr = parse_expression(lexer)?;
            expect_token!(expr.pos(), lexer, RPAREN => ())?;
            return Ok(WithPos{item: Print(expr), position: token.pos});
        },
        KEYWORD(LET) => {
            let (i, p) = expect_token!(token.pos, lexer, IDENT(i) => i)?;
            expect_token!(token.pos, lexer, ASSIGN => ())?;
            let expr = parse_expression(lexer)?;
            Ok(WithPos{item: Let(WithPos{item:i,position:p}, expr), position: token.pos})
        }
        t => Err(Error{item: ParseError::UnexpectedToken(t), position: token.pos})
    }
}

pub fn parse_statement_list<L: Lexer>(lexer: &mut L) -> Result<ast::StatementList> {
    let mut stmts: ast::StatementList = Vec::new();
    while let Some(_) = lexer.clone().peekable().peek() {
        let s = parse_statement(lexer)?;
        stmts.push(s);
    }
    Ok(stmts)
}
