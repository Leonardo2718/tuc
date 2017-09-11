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

use std::env;

pub fn is_flag_set(kind: &str, name: &str) -> bool {
    let args: Vec<_> = env::args().collect();
    let flag = String::from("-") + kind + name;
    args.contains(&flag)
}

pub fn get_positional(index: u32) -> Option<String> {
    let mut i = index;
    let args: Vec<String> = env::args().collect();
    for arg in args {
        let c = match arg.chars().nth(0) { Some(c) => c, None => return None };
        if c == '-' { continue; }
        else if i > 0 { i -= i; }
        else { return Some(arg); }
    }

    return None;
}
