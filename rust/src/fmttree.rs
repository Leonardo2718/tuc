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

#[derive(Debug,Clone)]
pub struct TreeFormat<'a> {
    indent: String,
    indent_segment: &'a str,
    point: &'a str,
}

impl<'a> TreeFormat<'a> {
    fn add_indent(&self) -> TreeFormat {
        let mut f = self.clone();
        f.indent += f.indent_segment;
        f
    }
}

pub trait Display {
    fn display_tree(&self) -> String {
        let f = TreeFormat{indent: "".to_string(), indent_segment: "  ", point: ""};
        self.display_sub_tree(f)
    }

    fn display_sub_tree(&self, f: TreeFormat) -> String {
        let mut n = format!("{}{}{}\n", f.indent, f.point, self.display_node());
        n += &self.display_children(f.add_indent());
        n
    }

    fn display_node(&self) -> String;

    fn display_children(&self, f: TreeFormat) -> String;
}
