/*
 * Copyright (c) 2018, 2019 Leonardo Banderali
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

mod expression_parser;
mod statement_parser;

use self::statement_parser::parse_statement_list;

use utils::*;
use tracing;
use token;
use lexer;
use lexer::Lexer;
use ast;

#[derive(Debug,Clone,PartialEq)]
pub enum ParseError {
    UnexpectedToken(token::TokenType),
    ExpectingMoreTokens,
    StatementParserError(statement_parser::ParseError),
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
            StatementParserError(_) => "Error occurred in statement parser",
            LexerError(_) => "Error occurred during lexing.",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        use self::ParseError::*;
        match *self {
            StatementParserError(ref e) => Some(e),
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

impl convert::From<statement_parser::Error> for Error {
    fn from(error: statement_parser::Error) -> Error {
        Error{item: ParseError::StatementParserError(error.item), position: error.position}
    }
}

pub type Result<T> = result::Result<T, Error>;

pub fn parse_program<L: Lexer>(lexer: L, traceContext: &mut tracing::TraceContext) -> Result<ast::Program> {
    use token::TokenType::*;
    use token::Token;
    let mut lexer = lexer.filter(|ref t| if let Ok(Token{token:COMMENT(_), pos:_}) = t { false } else { true });
    let mut tracer = tracing::Tracer::new(tracing::TraceOption::Parsing, traceContext);
    let ast = ast::Program{ body: parse_statement_list(&mut lexer, &mut tracer)? };
    use fmttree::Display;
    tracer.traceln("Generated AST:");
    tracer.trace(&ast.display_tree());
    Ok(ast)
}

#[cfg(test)]
mod test {
    use super::*;
}
