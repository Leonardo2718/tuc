/*
 * Copyright (c) 2019 Leonardo Banderali
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

use std::str::FromStr;
use std::hash::Hash;
use std::collections::HashSet;
use std::fs::File;
use std::io::BufWriter;
use std::io::Write;

macro_rules! define_trace_options {
    {$($s:expr => $v:ident),+} => {
        #[derive(Debug,Clone,Copy,PartialEq,Eq,Hash)]
        pub enum TraceOption {
            $($v),+
        }

        impl FromStr for TraceOption {
            type Err = ();

            fn from_str(s: &str) -> Result<Self, Self::Err> {
                use self::TraceOption::*;
                match s {
                $($s => Ok($v)),+,
                _ => Err(())
                }
            }
        }

        pub const TRACE_OPTIONS: &[&str] = &[$($s),+];
    }
}

define_trace_options! {
    "lexing" => Lexing,
    "parsing" => Parsing,
    "semantic" => Semantic,
    "ilgen" => IlGen,
    "codegen" => CodeGen,
    "all" => All
}


/// A struct containing the context used for tracing.
///
/// This struct encapsulates the information needed in order to
/// out put trace messages. However, it is not intended be used
/// directly. Instead, it allows different "tracers" to share the
/// same context/state, for example: which file is written to, etc.
pub struct TraceContext {
    trace_options: HashSet<TraceOption>,
    writer: Box<dyn Write>
}

impl TraceContext {
    pub fn new(trace_options: &[TraceOption], writer: Box<dyn Write>) -> TraceContext {
        let trace_options: HashSet<_> = trace_options.iter().cloned().collect();
        TraceContext{trace_options, writer}
    }
}

pub struct Tracer<'a> {
    option: TraceOption,
    context: &'a mut TraceContext
}

/// A struct encapsulating tracing functionality
impl<'a> Tracer<'a> {
    pub fn new(option: TraceOption, context: &mut TraceContext) -> Tracer {
        Tracer{option, context}
    }

    pub fn trace(&mut self, msg: &str) -> std::io::Result<()> {
        self.write(msg.as_bytes())?;
        Ok(())
    }

    pub fn traceln(&mut self, msg: &str) -> std::io::Result<()> {
        write!(self, "{}\n", msg)
    }
}

impl<'a> std::io::Write for Tracer<'a> {
    fn write(&mut self, buf: &[u8]) -> std::io::Result<usize> {
        if self.context.trace_options.contains(&self.option) || self.context.trace_options.contains(&TraceOption::All) {
            self.context.writer.write(buf)
        } else {
            Ok(0)
        }
    }
    
    fn flush(&mut self) -> std::io::Result<()> {
        self.context.writer.flush()
    }
}