/**
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

use std::io::prelude::Read;
use std::fs::File;
use std::io::BufReader;

mod utils;
mod token;
mod lexer;
mod ast;
mod parser;

extern crate argparse;

struct Options {
    source_path: String,
}

fn main() {
    let mut options = Options{source_path: "".to_string()};
    {
        use argparse as ap;
        let mut parser = ap::ArgumentParser::new();
        parser.set_description("Rust implementation of TUC (The U Compiler)");
        parser.refer(&mut options.source_path)
            .add_argument("source_path", ap::Store, "source file to be compiled").required();
        parser.parse_args_or_exit();
    }

    let mut source = String::new();
    {
        let f = File::open(options.source_path).unwrap();
        let mut reader = BufReader::new(f);
        reader.read_to_string(&mut source).unwrap();
    }

    let iter = lexer::TokenIterator::new(&source);
    let ast = parser::parse_program(iter).unwrap();

    println!("AST:\n{:?}", ast);
}