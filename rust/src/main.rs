/**
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

mod utils;
mod tracing;
mod types;
mod token;
mod lexer;
mod fmttree;
mod ast;
mod parser;
mod symtab;
mod semantics;
mod il;
mod genil;
mod wasmgen;

extern crate argparse;

use std::error;
use std::io::prelude::Read;
use std::fs::File;
use std::io::BufReader;
use std::io;

struct Options {
    source_path: String,
    trace: Vec<tracing::TraceOption>

}

macro_rules! handle_errors {
    ($expr:expr) => {
        match $expr {
            Ok(v) => v,
            Err(e) => {
                println!("Compilation error at {:?}:", e.pos());
                let mut err: &error::Error = &e.item.clone();
                loop {
                    println!("  error: {}", err.description());
                    if let Some(cause) = err.cause() { err = cause; }
                    else { println!("    {:?}", e); break; }
                }
                return;
            }
        }
    };
}

fn main() {
    let mut options = Options{source_path: "".to_string(), trace:vec![]};
    {
        let desc = format!("trace a step during compilation, must be one of: {:?}", tracing::TRACE_OPTIONS);
        {
            use argparse as ap;
            let mut parser = ap::ArgumentParser::new();
            parser.set_description("Rust implementation of TUC (The U Compiler)");
            parser.refer(&mut options.trace)
                .add_option(&["-t", "--trace"], ap::Collect, &desc);
            parser.refer(&mut options.source_path)
                .add_argument("source_path", ap::Store, "source file to be compiled").required();
            parser.parse_args_or_exit();
        }
    }

    let mut traceContext = tracing::TraceContext::new(&options.trace, Box::new(std::io::stderr()));

    let mut source = String::new();
    {
        let f = File::open(options.source_path).unwrap();
        let mut reader = BufReader::new(f);
        reader.read_to_string(&mut source).unwrap();
    }

    let iter = lexer::TokenIterator::new(&source);
    let mut ast = handle_errors!(parser::parse_program(iter, &mut traceContext));
    semantics::analyze_semantics(&mut ast, &mut traceContext).unwrap();

    let ast = ast; // make ast immutable

    let il = genil::gen_il(&ast, &mut traceContext).unwrap();

    let wat = wasmgen::generate_wat(&il, &mut traceContext).unwrap();
}