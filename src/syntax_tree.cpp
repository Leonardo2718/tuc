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

const tuc::SyntaxNode* tuc::SyntaxNode::parent() const noexcept {
    return parentNode;
}

tuc::SyntaxNode* tuc::SyntaxNode::parent() noexcept {
    return parentNode;
}

/*
returns child with index `i`
*/
const tuc::SyntaxNode* tuc::SyntaxNode::child(int i) const noexcept {
    return children[i].get();
}

tuc::SyntaxNode* tuc::SyntaxNode::child(int i) noexcept {
    return children[i].get();
}

int tuc::SyntaxNode::child_count() const noexcept {
    return children.size();
}

void tuc::SyntaxNode::append_child(NodeType _type, const TextEntity& _textValue) {
    children.push_back(std::make_unique<SyntaxNode>(_type, _textValue));
}

void tuc::SyntaxNode::append_child(const Token& _token) {
    children.push_back(std::make_unique<SyntaxNode>(_token));
}

/*  appends an already existing (allocated) child; since this node must "own" the child, move semantics *must*
    be used to transfer ownership.
*/
void tuc::SyntaxNode::append_child(std::unique_ptr<SyntaxNode>&& c) noexcept {
    children.push_back(std::move(c));
}

/*
removes and returns child with index `i`
*/
std::unique_ptr<tuc::SyntaxNode> tuc::SyntaxNode::remove(int i) {
    auto c = std::move(children[i]);
    children.erase(children.begin() + i);
    return c;
}

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

std::ostream& operator<< (std::ostream& os, const tuc::SyntaxNode* node) {
    auto nodeStack = std::vector<const tuc::SyntaxNode*>{};
    auto childIndexStack = std::vector<int>{};

    os << "[" << node->value() << "]\n";

    nodeStack.push_back(node);
    childIndexStack.push_back(0);
    while (!nodeStack.empty()) {
        if (childIndexStack.back() < nodeStack.back()->child_count()) {
            auto child = nodeStack.back()->child(childIndexStack.back());

            for (decltype(nodeStack)::size_type i = 0, s = nodeStack.size() - 1; i < s; i++) {
                if (childIndexStack[i] < nodeStack[i]->child_count())
                    os << " |   ";
                else
                    os << "     ";
            }

            os << " |-> [" << child->value() << "]\n";

            childIndexStack[childIndexStack.size() - 1]++;
            if (child->child_count() > 0) {
                nodeStack.push_back(child);
                childIndexStack.push_back(0);
            }
        }
        else {
            while (!nodeStack.empty() && childIndexStack.back() >= nodeStack.back()->child_count()) {
                nodeStack.pop_back();
                childIndexStack.pop_back();
            }

            if (!nodeStack.empty()) {
                for (decltype(nodeStack)::size_type i = 0, s = nodeStack.size(); i < s; i++) {
                    if (childIndexStack[i] < nodeStack[i]->child_count())
                        os << " |   ";
                    else
                        os << "     ";
                }
                os << "\n";
            }
        }
    }

    return os;
}

/*
puts a textual representation of a node hierarchy in an output stream
*/
std::ostream& operator<< (std::ostream& os, const std::unique_ptr<tuc::SyntaxNode, std::default_delete<tuc::SyntaxNode>>& node) {
    return os << node.get();
}
