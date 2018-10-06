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
use std::iter;
use std::str::Chars;
use std::num;
use std::error;
use std::result;
use std::clone;

use utils::*;
use token::*;

#[derive(Debug,Clone,PartialEq)]
struct CharPos {
    c: char,
    pos: Position,
}

#[derive(Debug,Clone)]
struct CharPosIter<'a> {
    current: char,
    pos: Position,
    iter: Chars<'a>,
}

impl<'a> CharPosIter<'a> {
    pub fn new<'b>(src: &'b str) -> CharPosIter<'b> {
        CharPosIter{current: '\0', pos: Position{pos: 0}, iter: src.chars()}
    }
}

impl<'a> iter::Iterator for CharPosIter<'a> {
    type Item = CharPos;

    fn next(&mut self) -> Option<Self::Item> {
        match self.iter.next() {
            Some(c) => {
                self.pos.pos += 1;
                self.current = c;
                Some(CharPos{c: self.current, pos: self.pos})
            }
            None => None
        }
    }
}

#[derive(Debug,Clone,PartialEq)]
pub enum LexerError {
    ExpectingMoreCharacters,
    UnexpectedCharacter(char),
    ParseIntError(num::ParseIntError),
    ParseFloatError(num::ParseFloatError),
}

// #[derive(Debug,Clone,PartialEq)]
// pub struct Error {
//     pub err: LexerError,
//     pub pos: Position,
// }
pub type Error = WithPos<LexerError>;

impl fmt::Display for Error {
    fn fmt(&self, f: &mut fmt::Formatter) -> fmt::Result {
        write!(f, "{:?}", self)
    }
}

impl error::Error for Error {
    fn description(&self) -> &str {
        use self::LexerError::*;
        match self.item {
            ExpectingMoreCharacters => "Lexer expected more characters for a complete token",
            UnexpectedCharacter(_) => "Lexer encountered an unexpected character",
            ParseIntError(_) => "Lexer failed to parse character sequence as an integer value (see cause)",
            ParseFloatError(_) => "Lexer failed to parse character sequence as a floating point value (see cause)",
        }
    }

    fn cause(&self) -> Option<&error::Error> {
        match self.item {
            LexerError::ParseIntError(ref e) => Some(e),
            LexerError::ParseFloatError(ref e) => Some(e),
            _ => None
        }
    }
}

pub type Result = result::Result<Token, Error>;

#[derive(Clone)]
pub struct TokenIterator<'a> {
    iter: CharPosIter<'a>,
    token_pos: Position,
}

impl<'a> TokenIterator<'a> {
    pub fn new<'b>(src: &'b str) -> TokenIterator<'b> {
        TokenIterator{iter: CharPosIter::new(src), token_pos: Position{pos: 0}}
    }

    fn match_next(&mut self, c: &CharPos) -> Result {
        use self::TokenType::*;

        macro_rules! emit {
            ($t:expr, $p:expr) => {
                Ok(Token{token: $t, pos: $p})
            };
        }

        match (c.c, c.pos) {
            ('+', p) => { self.iter.next(); emit!(OPERATOR(Operator::ADD), p) }
            ('-', p) => { self.iter.next(); emit!(OPERATOR(Operator::SUB), p) }
            ('*', p) => { self.iter.next(); emit!(OPERATOR(Operator::MUL), p) }
            ('/', p) => { self.iter.next(); emit!(OPERATOR(Operator::DIV), p) }
            ('(', p) => { self.iter.next(); emit!(LPAREN, p) }
            (')', p) => { self.iter.next(); emit!(RPAREN, p) }
            (';', p) => { self.iter.next(); emit!(SEMICOLON, p) }
            ('#', p) => {
                self.iter.next();
                let comment = self.iter.clone().map(|i| i.c).take_while(|c| c != &'\n').collect::<String>();
                self.iter.nth(comment.len()); // skipping len ensures \n is also skipped
                emit!(COMMENT(comment), p)
            }
            (c, p) if c.is_alphabetic() || c == '_' => {
                let ident = self.iter.clone().map(|i| i.c).take_while(|c| c.is_alphanumeric() || c == &'_').collect::<String>();
                self.iter.nth(ident.len() - 1);
                if let Some(k) = as_keyword(&ident) { emit!(KEYWORD(k), p) }
                else { emit!(IDENT(ident.clone()), p) }
            }
            (c, p) if c.is_digit(10) => {
                let int = self.iter.clone().map(|i| i.c).take_while(|c| c.is_digit(10)).collect::<String>();
                self.iter.nth(int.len() - 1);
                let int = int.parse::<i32>().map_err(|e| Error{item: LexerError::ParseIntError(e), position: p})?;
                emit!(CONST(Const::I32(int)), p)
            }
            (c, p) => { Err(Error{item: LexerError::UnexpectedCharacter(c), position: p}) }
        }
    }
}

impl<'a> Iterator for TokenIterator<'a> {
    type Item = Result;

    fn next(&mut self) -> Option<Self::Item> {
        loop {
            match self.iter.clone().peekable().peek() {
                Some(c) if c.c.is_whitespace() => { self.iter.next(); continue; }
                Some(c) => { return Some(self.match_next(c)); }
                _ => return None
            }
        }
    }
}

/*
A lexer can by any iterator that yeilds a lexer::Result as item.
The 'Lexer' trait expresses this requirement and is implemented
as a "trait alias" for the underlying iterator trait.
*/
pub trait Lexer: iter::Iterator<Item = Result> + clone::Clone {}
impl<T: iter::Iterator<Item = Result> + clone::Clone> Lexer for T {}

#[cfg(test)]
mod test {
    use super::*;
    use self::TokenType::*;

    macro_rules! assert_next_is {
        ($iter:expr, $token:expr, $pos:expr) => {
            let t = $iter.next();
            assert!(t.is_some());
            let t = t.unwrap();
            assert!(t.is_ok(), "Expected to be Ok(): t = {:?}", t);
            let t = t.unwrap();
            assert_eq!($token, t.token, "Wrong token type");
            assert_eq!(Position{pos: $pos}, t.pos, "Wrong token position");
        };
    }

    #[test]
    fn scan_ident_1() {
        let mut iter = TokenIterator::new("foo");
        assert_next_is!(iter, IDENT("foo".to_string()), 1);
    }

    #[test]
    fn scan_keyword_1() {
        use self::Keyword::*;
        let mut iter = TokenIterator::new("print");
        assert_next_is!(iter, KEYWORD(PRINT), 1);
    }

    #[test]
    fn scan_keyword_2() {
        use self::Keyword::*;
        let mut iter = TokenIterator::new("let");
        assert_next_is!(iter, KEYWORD(LET), 1);
    }

    #[test]
    fn scan_const_1() {
        let mut iter = TokenIterator::new("123");
        assert_next_is!(iter, CONST(Const::I32(123)), 1);
    }

    #[test]
    fn scan_comment_1() {
        let mut iter = TokenIterator::new("# some text");
        assert_next_is!(iter, COMMENT(" some text".to_string()), 1);
    }

    #[test]
    fn scan_expr_1() {
        use self::Operator::*;
        use self::Keyword::*;
        let mut iter = TokenIterator::new(" 1 / a + b * 3 ; # comment \n print ( x ) ;");
        assert_next_is!(iter, CONST(Const::I32(1)), 2);
        assert_next_is!(iter, OPERATOR(DIV), 4);
        assert_next_is!(iter, IDENT("a".to_string()), 6);
        assert_next_is!(iter, OPERATOR(ADD), 8);
        assert_next_is!(iter, IDENT("b".to_string()), 10);
        assert_next_is!(iter, OPERATOR(MUL), 12);
        assert_next_is!(iter, CONST(Const::I32(3)), 14);
        assert_next_is!(iter, SEMICOLON, 16);
        assert_next_is!(iter, COMMENT(" comment ".to_string()), 18);
        assert_next_is!(iter, KEYWORD(PRINT), 30);
        assert_next_is!(iter, LPAREN, 36);
        assert_next_is!(iter, IDENT("x".to_string()), 38);
        assert_next_is!(iter, RPAREN, 40);
        assert_next_is!(iter, SEMICOLON, 42);
    }
}