/*
Project: TUC
Author: Leonardo Banderali

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2017 Leonardo Banderali

License:

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.

*/

use source;
use debug;
use token::*;
use std::iter::*;
use std::slice::Iter;
use std::fmt;

macro_rules! traceln {
    ($($arg:tt)+) => { debug::trace_if("parse", &(format!($($arg)+) + "\n")); }
}

#[derive(Eq,PartialEq,Copy,Clone,Debug)]
pub enum BinaryOperator {
    Add,
    Sub,
    Mul,
    Div
}

impl fmt::Display for BinaryOperator {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

#[derive(Eq,PartialEq,Clone,Debug)]
pub enum Literal {
    Integer(String),
    Decimal(String)
}

impl fmt::Display for Literal {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

#[derive(Clone,Debug)]
pub enum ExprNode {
    Literal(Literal, source::SourceSnippit),
    Identifier(source::SourceSnippit),
    BinaryOperator(BinaryOperator, Box<ExprNode>, Box<ExprNode>, source::SourceSnippit)
}

#[derive(Clone,Debug)]
pub enum ASTNode {
    Expression(ExprNode),
    Proc(String, Vec<ASTNode>)
}

#[derive(Eq,PartialEq,Debug)]
pub enum Associativity {
    Left,
    Right
}

fn operator_prec(op: char) -> u32 {
    match op {
        '+' => 3,
        '-' => 3,
        '*' => 4,
        '/' => 4,
        ':' => 9,
        '=' => 10,
        _ => panic!("Attempted to get precidence of invalid operator {}", op)
    }
}

fn operator_assoc(op: char) -> Associativity {
    match op {
        '+' => Associativity::Left,
        '-' => Associativity::Left,
        '*' => Associativity::Left,
        '/' => Associativity::Left,
        ':' => Associativity::Left,
        '=' => Associativity::Left,
        _ => panic!("Attempted to get associativity of invalid operator {}", op)
    }
}

fn as_operator(op: &str) -> BinaryOperator {
  if op == "+" {BinaryOperator::Add}
  else if op == "-" {BinaryOperator::Sub}
  else if op == "*" {BinaryOperator::Mul}
  else if op == "/" {BinaryOperator::Div}
  else { panic!("Unkown operation {}", op) }
}

fn add_operator_node(buffer: &mut Vec<ExprNode>, token_snippit: &source::SourceSnippit) {
  let a1 = Box::from(buffer.pop().expect("Not enough tokens available for operator"));
  let a2 = Box::from(buffer.pop().expect("Not enough tokens available for operator"));
  buffer.push(ExprNode::BinaryOperator(as_operator(&token_snippit.snippit), a2, a1, token_snippit.clone()));
}

fn pop_high_prec_ops(buffer: &mut Vec<ExprNode>, op_stack: &mut Vec<Token>, token_snippit: &source::SourceSnippit) -> usize {
    let mut count = op_stack.len();
    let mut op_iter = op_stack.iter().rev().peekable();
    while let Some(&&Token(TokenType::Operator, ref s)) = op_iter.peek() {
        op_iter.next();
        traceln!("    Poping {} from operator stack", s.snippit);
        let op1 = token_snippit.snippit.chars().nth(0).unwrap();
        let op2 = s.snippit.chars().nth(0).unwrap();
        if (operator_assoc(op1) == Associativity::Left && operator_prec(op1) <= operator_prec(op2)) ||
           (operator_assoc(op1) == Associativity::Right && operator_prec(op1) < operator_prec(op2)) {
            count -= 1;
            add_operator_node(buffer, s)
        }
        else {
            break;
        }
    }
    return count;
}

fn parse_expr <'a>(token_iter: &mut Peekable<Iter<'a, Token>>) -> ExprNode {
    let mut buffer: Vec<ExprNode> = Vec::new();
    let mut op_stack: Vec<Token> = Vec::new();

    loop {
        let token = token_iter.next().expect("Expected a token");
        traceln!("  Found {} token: {}", token.0, token.1.snippit);
        match *token {
            Token(TokenType::Semicolon, _) => {
                while let Some(op) = op_stack.pop() {
                    match op {
                        Token(TokenType::Operator, ref s) => add_operator_node(&mut buffer, s),
                        Token(TokenType::LParen, ref s) => panic!("No ')' found for '(' at {}", s.position),
                        Token(t, s) => panic!("Unexpected {} token '{}' on operator stack from {}", t, s.snippit, s.position)
                    }
                }
                traceln!("    End of expression");
                break;
            },
            Token(TokenType::Integer, ref snippit) => {
                traceln!("    Adding to node buffer");
                buffer.push(ExprNode::Literal(Literal::Integer(snippit.snippit.clone()), snippit.clone()));
            },
            Token(TokenType::Decimal, ref snippit) => {
                traceln!("    Adding to node buffer");
                buffer.push(ExprNode::Literal(Literal::Decimal(snippit.snippit.clone()), snippit.clone()))
            },
            Token(TokenType::Identifier, ref snippit) => {
                traceln!("    Adding to node buffer");
                buffer.push(ExprNode::Identifier(snippit.clone()))
            },
            Token(TokenType::Operator, ref snippit) => {
                let count = pop_high_prec_ops(&mut buffer, &mut op_stack, snippit);
                op_stack.drain(count..);
                traceln!("    Pushing onto operator stack");
                op_stack.push(token.clone());
            },
            Token(TokenType::LParen, _) => {
                traceln!("    Pushing onto operator stack");
                op_stack.push(token.clone())
            },
            Token(TokenType::RParen, ref snippit) => {
                loop {
                    match op_stack.pop() {
                        Some(Token(TokenType::Operator, s)) => {
                            traceln!("    Poping {} from operator stack", s.snippit);
                            add_operator_node(&mut buffer, &s);
                        },
                        Some(Token(TokenType::LParen, _)) => {
                            traceln!("    Found matching parentheses on operator stack");
                            break;
                        },
                        Some(_) => panic!("Found non-operator and non-left-parenthesis on operator stack {:?}", token),
                        None => panic!("No matching parenthesis found for ')' at {}", snippit.position)
                    };
                }
            },
            Token(t, ref s) => panic!("Expected Expression token, found {} token from {}", t, s.snippit)
        }
    }

    assert_eq!(1, buffer.len());
    return buffer.pop().expect("Expression token buffer cannot be empty!");
}

pub fn parse(proc_name: &str, tokens: &[Token]) -> Box<ASTNode> {
    let mut nodes: Vec<ASTNode> = Vec::new();

    traceln!("=== Parsing token list ===");
    let mut token_iter = tokens.iter().peekable();
    while let Some(token) = token_iter.clone().peek() {
        traceln!("Next token `{}` is {}", token.1.snippit, token.0);
        let node = match **token {
            Token(TokenType::Integer, _) => {
                traceln!("  Parsing as expression");
                ASTNode::Expression(parse_expr(&mut token_iter))
            },
            Token(TokenType::Decimal, _) => {
                traceln!("  Parsing as expression");
                ASTNode::Expression(parse_expr(&mut token_iter))
              },
            Token(TokenType::Identifier, _) => {
                traceln!("  Parsing as expression");
                ASTNode::Expression(parse_expr(&mut token_iter))
            },
            Token(TokenType::LParen, _) => {
                traceln!("  Parsing as expression");
                ASTNode::Expression(parse_expr(&mut token_iter))
            },
            Token(TokenType::Semicolon, _) => {
                traceln!("  - Ignoring");
                token_iter.next(); continue
            },
            Token(TokenType::LComment, _) => {
                traceln!("  - Ignoring");
                token_iter.next(); continue
            },
            _ => panic!("Unexpected token")
        };
        nodes.push(node);
    }

    return Box::new(ASTNode::Proc(String::from(proc_name), nodes));
}

macro_rules! print_node {
    ($node:expr) => (println!("-{}", $node))
}

pub fn print_ast_expr( lead: String, node: &ExprNode) {
    match *node {
        ExprNode::Literal(ref lit, _) => {
            print_node!(lit);
        },
        ExprNode::Identifier(ref snippit) => {
            print_node!(snippit.snippit);
        },
        ExprNode::BinaryOperator(ref op, ref left, ref right, _) => {
            print_node!(op);
            print!("{}|-", lead.clone());
            print_ast_expr(lead.clone() + "| ", left);
            print!("{}`-", lead.clone());
            print_ast_expr(lead.clone() + "  ", right);
        }
    }
}
pub fn print_ast(lead: String, node: &ASTNode) {
    match *node {
        ASTNode::Expression(ref expr) => print_ast_expr(lead, expr),
        ASTNode::Proc(ref name, ref children) => {
            print!("{}", lead.clone());
            print_node!(name);
            for ref c in children.iter().take(children.len() - 1) {
                print!("{}|-", lead.clone());
                print_ast(lead.clone() + "| ", c);
            }
            print!("{}`-", lead.clone());
            print_ast(lead.clone() + "  ", children.iter().last().unwrap());
        }
    }
}

#[cfg(test)]
mod test {
    use super::*;

    fn create_token(token_type: TokenType, snippit: &str) -> Token {
        let pos = source::FilePosition {file_path: String::from("none"), index: 0, line: 0, column: 0};
        let snip = source::SourceSnippit {snippit: String::from(snippit), position: pos};
        Token(token_type, snip)
    }

    #[test]
    fn simple_add() {
        let source_tokens =
            [ create_token(TokenType::Integer, "1")
            , create_token(TokenType::Operator, "+")
            , create_token(TokenType::Integer, "1")
            , create_token(TokenType::Semicolon, ";")
            ];
        parse("simple_add", &source_tokens);
    }
}
