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

use utils::*;
use token;
use lexer;
use lexer::Lexer;
use lexer::TokenIterator;
use ast;

#[derive(Debug,Clone,PartialEq)]
pub enum ParseError {
    UnexpectedToken(token::TokenType),
    LexerError(lexer::Error)
}

type Error = WithPos<ParseError>;

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl error::Error for Error {
    fn description(&self) -> &str {
        use self::ParseError::*;
        match **self {
            UnexpectedToken(_) => "Encountered unexpected token while parsing.",
            LexerError(_) => "Error occurred during lexing.",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        use self::ParseError::*;
        match **self {
            LexerError(ref e) => Some(e),
            _ => None
        }
    }
}

impl convert::From<lexer::Error> for Error {
    fn from(error: lexer::Error) -> Error {
        Error{item: ParseError::LexerError(error.clone()), position: error.position}
    }
}

type Result<T> = result::Result<T, Error>;

fn parse_expression<L: Lexer>(lexer: &mut L) -> Result<WithPos<ast::WithType<ast::Expression>>> {
    use utils::Const::*;
    use ast::Expression::*;
    use ast::Type;
    Ok(WithPos{item: ast::WithType{item: Literal(I32(3)), t: Type::I32}, position: Position{pos:0}})
}

fn parse_statement<L: Lexer>(lexer: &mut L) -> Result<WithPos<ast::Statement>>  {
    use token::TokenType::*;
    use token::Keyword::*;
    use token::Operator::*;
    use ast::Statement::*;

    let token = lexer.next().unwrap()?;
    match token.token {
        KEYWORD(PRINT) => {
            let expr = parse_expression(lexer)?;
            return Ok(WithPos{item: Print(expr), position: token.pos});
        }
        t => Err(Error{item: ParseError::UnexpectedToken(t), position: token.pos})
    }
}

fn parse_statement_list<L: Lexer>(lexer: &mut L) -> Result<ast::StatementList> {
    let mut stmts: ast::StatementList = Vec::new();
    while let Some(_) = lexer.clone().peekable().peek() {
        let s = parse_statement(lexer)?;
        stmts.push(s);
    }
    Ok(stmts)
}

pub fn parse_program<L: Lexer>(mut lexer: L) -> Result<ast::Program> {
    use token::TokenType::*;
    use token::Token;
    let mut lexer = lexer.filter(|ref t| if let Ok(Token{token:COMMENT(_), pos:_}) = t { false } else { true });
    Ok(ast::Program{ body: parse_statement_list(&mut lexer)? })
}

#[cfg(test)]
mod test {
    use super::*;
}