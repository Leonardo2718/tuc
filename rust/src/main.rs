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

mod source;
mod token;
mod ast;
mod debug;
mod ir;

use std::ops::Index;
use std::error::Error;
use std::io::Read;

macro_rules! print_on_debug {
    ($flag:expr, $msg:expr, $($args:expr),*) => (
        if debug::debug_flag($flag) { println!($msg, $($args),*) }
    );
}

fn main() {
    // get the cli arguments
    let args: Vec<_> = std::env::args().collect();

    // get the first argument (source file path)
    let path = std::path::Path::new(args.index(1));
    print_on_debug!("file","source file: {}", path.display());

    // open the source file
    let mut src_file = match std::fs::File::open(path) {
        Err(why) => panic!("Failed to open file {}: {}", path.display(), why.description()),
        Ok(file) => file
    };

    // read the source file
    let mut src_code = String::new();
    match src_file.read_to_string(&mut src_code) {
        Err(why) => panic!("Failed to read file {}, {}", path.display(), why.description()),
        Ok(_) => print_on_debug!("source", "source code:\n```\n{}\n```", src_code)
    }

    // tokenize source code
    let tokens = token::lex(&src_code, match path.to_str() {Some(p) => p, _ => panic!("Can't convert path to a string.")} );

    // print tokens
    if debug::debug_flag("tokens") {
        println!("list of tokens:");
        for &token::Token(t, ref lexeme) in &tokens {
            println!("{:width$} {:width$}\n`-[{}]", format!("{}", t), lexeme.snippit, lexeme.position, width = 12);
        }
    }

    // parse tokens
    let parse_tree = ast::parse("main", tokens.as_slice());

    //print_on_debug!("ast", "{:?}", parse_tree);
    if debug::debug_flag("ast") {
        println!("abstract syntax tree:");
        ast::print_ast(String::from(""), &parse_tree);
    }

    // generate IR for the program
    let intermediate = ir::gen_ir(&parse_tree);

    // print IR for debugging
    if debug::debug_flag("ir") {
        for inst in intermediate.instructions {
            println!("{}", inst);
        }
    }
}
