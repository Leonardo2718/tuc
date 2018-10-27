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
    ExpectingMoreTokens,
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
            ExpectingMoreTokens => "Expected to find more tokens but got nothing",
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

#[derive(Debug,Clone,Copy,PartialEq,Eq)]
enum Associativity {
    Left,
    Rigth
}

impl token::Operator {
    fn precedence(self) -> u32 {
        use token::Operator::*;
        match self {
            ADD => 2,
            SUB => 2,
            MUL => 3,
            DIV => 3,
        }
    }

    fn associativity(self) -> Associativity {
        Associativity::Left
    }
}

/*
Dijkstra's shunting-yard algorithm:

while there are tokens to be read:
    read a token.
    if the token is a number, then:
        push it to the output queue.
    if the token is a function then:
        push it onto the operator stack 
    if the token is an operator, then:
        while ((there is a function at the top of the operator stack)
               or (there is an operator at the top of the operator stack with greater precedence)
               or (the operator at the top of the operator stack has equal precedence and is left associative))
              and (the operator at the top of the operator stack is not a left bracket):
            pop operators from the operator stack onto the output queue.
        push it onto the operator stack.
    if the token is a left bracket (i.e. "("), then:
        push it onto the operator stack.
    if the token is a right bracket (i.e. ")"), then:
        while the operator at the top of the operator stack is not a left bracket:
            pop the operator from the operator stack onto the output queue.
        pop the left bracket from the stack.
        /* if the stack runs out without finding a left bracket, then there are mismatched parentheses. */
if there are no more tokens to read:
    while there are still operator tokens on the stack:
        /* if the operator token on the top of the stack is a bracket, then there are mismatched parentheses. */
        pop the operator from the operator stack onto the output queue.
exit.
*/
fn parse_expression<L: Lexer>(lexer: &mut L) -> Result<WithPos<ast::WithType<ast::Expression>>> {
    use utils::Const::*;
    use token::TokenType::*;
    use ast::Expression::*;
    use ast::Type;

    let mut expr_stack: Vec<WithPos<ast::WithType<ast::Expression>>> = Vec::new();
    let mut op_stack: Vec<token::Token> = Vec::new();

    macro_rules! push_expr {
        ($token:expr, $type:expr, $expr:ident, $($args:expr),* ) => {
            expr_stack.push(WithPos{item:ast::WithType{item:$expr($($args),*), t:$type}, position:$token.pos});
        };
    }

    macro_rules! optoken2opast {
        ($optoken:expr) => {{
            use token::Operator::*;
            use ast::BinaryOperator::*;
            match $optoken {
                ADD => Add,
                SUB => Sub,
                MUL => Mul,
                DIV => Div
            }
        }};
    }

    macro_rules! pop_op {
        ($optoken:expr,$t:expr) => {
            let rhs = expr_stack.pop().ok_or(WithPos{item:ParseError::UnexpectedToken($t.token.clone()), position:$t.pos})?;
            let lhs = expr_stack.pop().ok_or(WithPos{item:ParseError::UnexpectedToken($t.token.clone()), position:$t.pos})?;
            push_expr!($t, Type::Unknown, BinaryOp, optoken2opast!($optoken), Box::new(lhs), Box::new(rhs));
        };
    }

    while let Some(next) = lexer.clone().peekable().peek() {
        let next = next.clone()?;

        match next.token {
            IDENT(s) => { lexer.next(); push_expr!(next, Type::Unknown, Identifier, s); },
            CONST(c) => { lexer.next(); match c {
                I32(_) => push_expr!(next, Type::I32, Literal, c)
            };},
            OPERATOR(o) => {
                lexer.next();
                while let Some(OPERATOR(p)) = op_stack.iter().rev().map(|t| t.token.clone()).next() {
                    op_stack.pop();
                    if p.precedence() > o.precedence() { pop_op!(o,next); }
                    else if p.precedence() == o.precedence() && p.associativity() == Associativity::Left { pop_op!(o,next); }
                    else { break; }
                }
                op_stack.push(next);
            },
            LPAREN => {
                lexer.next();
                op_stack.push(next);
            },
            RPAREN => {
                let mut inexpr = false;
                while let Some(t) = op_stack.pop() {
                    if let OPERATOR(p) = t.token { pop_op!(p,t); }
                    else if LPAREN == t.token { inexpr = true; break; }
                    else { return Err(WithPos{item:ParseError::UnexpectedToken(t.clone().token),position:t.pos}); }
                }

                if !inexpr && expr_stack.len() == 1 { return Ok(expr_stack.pop().unwrap()); }
                else { lexer.next(); }
            },
            t => {
                break;
            },
        }
    }

    while let Some(t) = op_stack.pop() {
        if let OPERATOR(p) = t.token { pop_op!(p,t); }
        else { return Err(WithPos{item:ParseError::UnexpectedToken(t.clone().token),position:t.pos}); }
    }

    if expr_stack.len() == 1 {
        Ok(expr_stack.pop().unwrap())
    }
    else {
        Err(WithPos{item:ParseError::ExpectingMoreTokens, position: Position{pos:0}})
    }
}

macro_rules! expect_token {
    ($pos:expr, $lexer:expr, $token:tt) => {
        if let Some(next) = $lexer.next() {
            match next?.token {
                token::TokenType::$token => Ok(()),
                t => Err(WithPos{item: ParseError::UnexpectedToken(t), position: $pos})
            }
        }
        else {
            Err(WithPos{item: ParseError::ExpectingMoreTokens, position: $pos})
        }
    };
}

fn parse_statement<L: Lexer>(lexer: &mut L) -> Result<WithPos<ast::Statement>>  {
    use token::TokenType::*;
    use token::Keyword::*;
    use token::Operator::*;
    use ast::Statement::*;

    let token = lexer.next().unwrap()?;
    match token.token {
        KEYWORD(PRINT) => {
            expect_token!(token.pos, lexer, LPAREN)?;
            let expr = parse_expression(lexer)?;
            expect_token!(expr.pos(), lexer, RPAREN)?;
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

pub fn parse_program<L: Lexer>(lexer: L) -> Result<ast::Program> {
    use token::TokenType::*;
    use token::Token;
    let mut lexer = lexer.filter(|ref t| if let Ok(Token{token:COMMENT(_), pos:_}) = t { false } else { true });
    Ok(ast::Program{ body: parse_statement_list(&mut lexer)? })
}

#[cfg(test)]
mod test {
    use super::*;
}