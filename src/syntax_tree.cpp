/*
Project: TUC
File: syntax_tree.cpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: January 17, 2016

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2016 Leonardo Banderali

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

// project headers
#include "syntax_tree.hpp"



//~class implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

tuc::SyntaxNode::SyntaxNode(NodeType _type) : syntaxNodeType{_type} {}

tuc::SyntaxNode::SyntaxNode(NodeType _type, const TextEntity& _textValue)
: syntaxNodeType{_type}, textValue{_textValue} {}

/*
constructs a node from a syntax token
*/
tuc::SyntaxNode::SyntaxNode(const Token& _token)
: syntaxNodeType{_token.type()}, textValue{_token.text()} {}

tuc::NodeType tuc::SyntaxNode::type() const noexcept {
    return syntaxNodeType;
}

std::string tuc::SyntaxNode::value() const noexcept {
    return textValue.text();
}

tuc::FilePosition tuc::SyntaxNode::position() const {
    return textValue.position();
}

tuc::TextEntity tuc::SyntaxNode::text() const noexcept {
    return textValue;
}



//~overloaded functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
puts textual representation of node in an output stream
*/
std::ostream& operator<< (std::ostream& os, const tuc::SyntaxNode& node) {
    os << "[" << node.value() << "]";
    return os;
}
