/*
Project: TUC
File: syntax_tree.cpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: September 25, 2015

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

/*
returns a new node without putting it in the tree
*/
tuc::SyntaxTree::SyntaxNode tuc::SyntaxTree::newNode(const Token& token) {
    //tempNodes.push_back(std::make_unique<InternalSyntaxNode>(token));
    //return SyntaxNode{tempNodes.back().get()};
    return SyntaxNode{new InternalSyntaxNode{token}};
}

/*
appends a new child to the current node
*/
tuc::SyntaxTree::SyntaxNode tuc::SyntaxTree::appendChild(SyntaxNode node, const Token& token) {
    auto n = node.internalNode;
    n->children.push_back(std::make_unique<InternalSyntaxNode>(token, n));
    return SyntaxNode{node.internalNode->children.back().get()};
}

/*
appends node n as a child of origin
*/
void tuc::SyntaxTree::appendNode(SyntaxNode origin, SyntaxNode node) {
    auto n = node.internalNode;
    origin.internalNode->children.push_back(std::unique_ptr<InternalSyntaxNode>(n));
    n->parent = origin.internalNode;
}

/*
prepends a new child to the current node
*/
tuc::SyntaxTree::SyntaxNode tuc::SyntaxTree::prependChild(SyntaxNode node, const Token& token) {
    auto n = node.internalNode;
    n->children.insert(n->children.cbegin(), std::make_unique<InternalSyntaxNode>(token, n));
    return SyntaxNode{node.internalNode->children.front().get()};
}

/*
returns the number of child nodes
*/
int tuc::SyntaxTree::childCount(SyntaxNode node) const {
    return node.internalNode->children.size();
}

/*
returns a particular child node
*/
tuc::SyntaxTree::SyntaxNode tuc::SyntaxTree::child(SyntaxNode node, int childNumber) const {
    return SyntaxNode{node.internalNode->children[childNumber].get()};
}

/*
returns all child nodes
*/
std::vector<tuc::SyntaxTree::SyntaxNode> tuc::SyntaxTree::children(SyntaxNode node) const {
    const auto& cv = node.internalNode->children;
    auto rv = std::vector<tuc::SyntaxTree::SyntaxNode>{};
    rv.reserve(cv.size());
    for (const auto& c : cv)
        rv.push_back(SyntaxNode{c.get()});
    return rv;
}

/*
returns the parent node
*/
tuc::SyntaxTree::SyntaxNode tuc::SyntaxTree::parent(SyntaxNode node) const {
    return SyntaxNode{node.internalNode->parent};
}

/*
returns the root node of the tree
*/
tuc::SyntaxTree::SyntaxNode tuc::SyntaxTree::root() const {
    return SyntaxNode{rootNode.get()};
}



tuc::SyntaxTree::SyntaxNode::SyntaxNode(InternalSyntaxNode* _internalNode)
    : internalNode{_internalNode} {}

tuc::Token tuc::SyntaxTree::SyntaxNode::token() const {
    return internalNode->token;
}



int tuc::SyntaxTree::SyntaxNode::childCount() const {
    return internalNode->children.size();
}



tuc::SyntaxTree::InternalSyntaxNode::InternalSyntaxNode(const Token& _token, InternalSyntaxNode* _parent)
    : token{_token}, parent{_parent} {}
