/*
Project: TUC
File: syntax_tree.cpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: October 6, 2015

Description:
    TUC is a simple, experimental compiler designed for learning and experimenting.
    It is not intended to have any useful purpose other than being a way to learn
    how compilers work.

Copyright (C) 2015 Leonardo Banderali

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

tuc::SyntaxNode::SyntaxNode(NodeType _type, const std::string& _value, int _pos) :
    syntaxNodeType{_type}, textValue{_value}, pos{_pos} {}

/*
constructs a node from a syntax token
*/
tuc::SyntaxNode::SyntaxNode(const Token& _token)
: textValue{_token.lexeme()}, pos{_token.position()} {
    switch (_token.type()) {
    case TokenType::ADD:        syntaxNodeType = NodeType::ADD;break;
    case TokenType::SUBTRACT:   syntaxNodeType = NodeType::SUBTRACT;break;
    case TokenType::MULTIPLY:   syntaxNodeType = NodeType::MULTIPLY;break;
    case TokenType::DIVIDE:     syntaxNodeType = NodeType::DIVIDE;break;
    case TokenType::INTEGER:    syntaxNodeType = NodeType::INTEGER;break;
    case TokenType::SEMICOL:    syntaxNodeType = NodeType::SEMICOL;break;
    default:                    syntaxNodeType = NodeType::UNKNOWN;
    }
}

tuc::SyntaxNode* tuc::SyntaxNode::parent() noexcept {
    return parentNode;
}

/*
returns child with index `i`
*/
tuc::SyntaxNode* tuc::SyntaxNode::child(int i) noexcept {
    return children[i].get();
}

int tuc::SyntaxNode::child_count() const noexcept {
    return children.size();
}

void tuc::SyntaxNode::append_child(NodeType _type, const std::string& _value, int _pos) {
    children.push_back(std::make_unique<SyntaxNode>(_type, _value, _pos));
}

void tuc::SyntaxNode::appedn_child(const Token& _token) {
    children.push_back(std::make_unique<SyntaxNode>(_token));
}

/*  appends an already existing (allocated) child; since this node must "own" the child, move semantics *must*
    be used to transfer ownership.
*/
void tuc::SyntaxNode::append_child(std::unique_ptr<SyntaxNode>&& c) noexcept {
    children.push_back(std::move(c));
}

tuc::SyntaxNode::NodeType tuc::SyntaxNode::type() const noexcept {
    return syntaxNodeType;
}

bool tuc::SyntaxNode::is_operator() const noexcept {
    return syntaxNodeType == NodeType::ADD || syntaxNodeType == NodeType::SUBTRACT ||
            syntaxNodeType == NodeType::MULTIPLY || syntaxNodeType == NodeType::DIVIDE;
}

std::string tuc::SyntaxNode::value() const noexcept {
    return textValue;
}

int tuc::SyntaxNode::position() const noexcept {
    return pos;
}
