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

use debug;
use source;

use std::fmt;

macro_rules! traceln {
    ($($arg:tt)+) => { if debug::trace_enabled("lex") { println!($($arg)+) } }
}

#[derive(Debug, Eq, PartialEq, Copy, Clone)]
pub enum TokenType {
    LComment,
    Keyword,
    Operator,
    Integer,
    Decimal,
    LParen,
    RParen,
    Identifier,
    Semicolon,
}

impl fmt::Display for TokenType {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", *self)
    }
}

#[derive(Eq,PartialEq,Clone,Debug)]
pub struct Token(pub TokenType, pub source::SourceSnippit);

impl fmt::Display for Token {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "Token({}, {})", (*self).0, (*self).1)
    }
}

pub type TokenList = Vec<Token>;

#[derive(Debug)]
enum State {
    EatChar,
    EatLine,
    EatIdentifier,
    EatNumber
}

impl fmt::Display for State {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", *self)
    }
}

pub fn lex(source: &str, source_path: &str) -> TokenList {
    traceln!("Lexing code from '{}':\n```\n{}\n```", source_path, source);

    let operators: [char; 8] = [':', '=', '-', '+', '*', '/', '<', '>'];
    let keywords: [&str; 1] = ["int"];

    let mut buffer = String::new();
    let mut tokens = TokenList::new();
    let mut state = State::EatChar;
    let mut token_type = TokenType::LComment;
    let mut token_ready = false;
    let mut move_line = false;
    let mut index: usize = 0;
    let mut line:  usize = 1;
    let mut column: usize = 1;
    let mut position = source::FilePosition {file_path: String::from(source_path), index: index, line: line, column: column};

    let mut chars = source.chars().peekable();
    loop {
        let current_char = match chars.next() {
            None => break,
            Some(c) => c
        };
        traceln!("Reading character {:?} <index={},line={},column={}>", current_char, index, line, column);
        traceln!("  current state: {}", state);

        match state {
            State::EatChar => {
                position.index = index;
                position.line = line;
                position.column = column;

                if current_char.is_whitespace() {
                    if current_char == '\n' { move_line = true; traceln!("  is newline -> incrementing line number"); }
                    else { traceln!("  is whitespace -> ignoring"); }
                }
                else {
                    buffer.push(current_char);
                    traceln!("  pushing to buffer [{}]", buffer);
                    match current_char {
                        '#' => {token_type = TokenType::LComment; state = State::EatLine;},
                        '(' => {token_type = TokenType::LParen; token_ready = true;},
                        ')' => {token_type = TokenType::RParen; token_ready = true;},
                        ';' => {token_type = TokenType::Semicolon; token_ready = true;},
                        _ if current_char.is_alphabetic() => {token_type = TokenType::Identifier; state = State::EatIdentifier;},
                        _ if current_char.is_numeric() => {
                            token_type = TokenType::Integer;
                            let peeked_char = chars.peek();
                            traceln!("  peeking next character -> found '{}'", match peeked_char { Some(c) => *c, _ => ' '} );
                            match peeked_char {
                                Some(&'.') => {state = State::EatNumber; token_type = TokenType::Decimal;},
                                Some(c) if c.is_numeric() => {state = State::EatNumber;},
                                _ => {token_ready = true;}
                            };
                        },
                        _ if operators.contains(&current_char) =>
                            {token_type = TokenType::Operator; token_ready = true;},
                        _ => panic!("Unidentified character `{}`", current_char)
                    }

                    traceln!("  guessing is {}", token_type);
                }
            },
            State::EatLine => {
                buffer.push(current_char);
                traceln!("  pushing to buffer [{}]", buffer);
                let peeked_char = chars.peek();
                traceln!("  peeking next character -> found '{}'", match peeked_char { Some(c) => *c, _ => ' '} );
                token_ready = match peeked_char {Some(&'\n') => true, None => true,_ => false}
            },
            State::EatIdentifier => {
                buffer.push(current_char);
                traceln!("  pushing to buffer [{}]", buffer);
                let peeked_char = chars.peek();
                traceln!("  peeking next character -> found '{}'", match peeked_char { Some(c) => *c, _ => ' '} );
                match peeked_char {
                    Some(next_c) if !next_c.is_alphanumeric() && next_c != & '_' => {
                        if keywords.contains(&buffer.as_str()) {
                            token_type = TokenType::Keyword;
                        }
                        token_ready = true;
                    },
                    _ => {}
                };
            },
            State::EatNumber => {
                buffer.push(current_char);
                traceln!("  pushing to buffer [{}]", buffer);
                let peeked_char = chars.peek();
                traceln!("  peeking next character -> found '{}'", match peeked_char { Some(c) => *c, _ => ' '} );
                match peeked_char {
                    Some(&'.') if token_type == TokenType::Integer => { token_type = TokenType::Decimal; },
                    Some(&'.') if token_type != TokenType::Integer => { panic!("Too many decimal points!"); },
                    Some(next_c) if next_c.is_numeric() => { },
                    _ => { token_ready = true; }
                }
            }
        }

        index += 1;
        column += 1;

        if token_ready {
            let snip = source::SourceSnippit {snippit: buffer.clone(), position: position.clone()};
            let token = Token(token_type,snip);
            traceln!("  found complete token: {}", token);
            tokens.push(token);
            buffer.clear();
            state = State::EatChar;
            token_ready = false;
        }

        traceln!("  next state is {}", state);

        if move_line {
            line += 1;
            column = 1;
            move_line = false;
        }
    }

    return tokens;
}

#[cfg(test)]
mod tests {
    use super::*;

    fn create_token(token_type: TokenType, snippit: &str, source_path: &str, index: usize, line: usize, column: usize) -> Token {
        let pos = source::FilePosition {file_path: String::from(source_path), index: index, line: line, column: column};
        let snip = source::SourceSnippit {snippit: String::from(snippit), position: pos};
        Token(token_type, snip)
    }

    #[test]
    fn test_single_line_with_comment() {
        let source_code = "foo : int = (1.0 + 1)*2; # comment";
        let source_file = "file.foo";
        let expected_tokens =
            [ create_token(TokenType::Identifier, "foo", source_file, 0, 1, 1)
            , create_token(TokenType::Operator, ":", source_file, 4, 1, 5)
            , create_token(TokenType::Keyword, "int", source_file, 6, 1, 7)
            , create_token(TokenType::Operator, "=", source_file, 10, 1, 11)
            , create_token(TokenType::LParen, "(", source_file, 12, 1, 13)
            , create_token(TokenType::Decimal, "1.0", source_file, 13, 1, 14)
            , create_token(TokenType::Operator, "+", source_file, 17, 1, 18)
            , create_token(TokenType::Integer, "1", source_file, 19, 1, 20)
            , create_token(TokenType::RParen, ")", source_file, 20, 1, 21)
            , create_token(TokenType::Operator, "*", source_file, 21, 1, 22)
            , create_token(TokenType::Integer, "2", source_file, 22, 1, 23)
            , create_token(TokenType::Semicolon, ";", source_file, 23, 1, 24)
            , create_token(TokenType::LComment, "# comment", source_file, 25, 1, 26)
            ];

        let tokens = lex(source_code, source_file);

        //assert_eq!(expected_tokens.len(), tokens.len());
        for i in 0..expected_tokens.len() {
            assert_eq!(expected_tokens[i], tokens[i]);
        }
    }

    #[test]
    fn test_multi_line() {
        let source_code = "1+2; # line 1\n3; # line 2";
        let source_file = "file.foo";
        let expected_tokens =
            [ create_token(TokenType::Integer, "1", source_file, 0, 1, 1)
            , create_token(TokenType::Operator, "+", source_file, 1, 1, 2)
            , create_token(TokenType::Integer, "2", source_file, 2, 1, 3)
            , create_token(TokenType::Semicolon, ";", source_file, 3, 1, 4)
            , create_token(TokenType::LComment, "# line 1", source_file, 5, 1, 6)
            , create_token(TokenType::Integer, "3", source_file, 14, 2, 1)
            , create_token(TokenType::Semicolon, ";", source_file, 15, 2, 2)
            , create_token(TokenType::LComment, "# line 2", source_file, 17, 2, 4)
            ];

        let tokens = lex(source_code, source_file);

        //assert_eq!(expected_tokens.len(), tokens.len());
        for i in 0..expected_tokens.len() {
            assert_eq!(expected_tokens[i], tokens[i]);
        }
    }
}
