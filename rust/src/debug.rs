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

use args;

use std::fs::OpenOptions;
use std::io::Write;

pub fn debug_flag(flag_name: &str) -> bool {
    args::is_flag_set("d", flag_name)
}

pub fn trace_enabled(item: &str) -> bool {
    args::is_flag_set("t", item) || args::is_flag_set("t", "all")
}

pub fn trace_msg(message: &str) {
    match args::get_value("log") {
        Some(path_str) => {
            let mut file = OpenOptions::new().append(true).write(true).create(true).open(&path_str).expect("Failed to open log file.");
            file.write_all(message.as_bytes()).expect("Failed to write to log file.");
        },
        None => { print!("{}", message); }
    }
}

pub fn trace_if(flag: &str, message: &str) {
    if trace_enabled(flag) { trace_msg(message); }
}