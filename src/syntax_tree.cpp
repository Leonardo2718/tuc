/*
Project: TUC
File: syntax_tree.cpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: November 8, 2015

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
#include "compiler_exceptions.hpp"

// standard libraries
#include <stdexcept>
#include <sstream>
#include <iostream>



//~class implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

tuc::SyntaxNode::SyntaxNode(NodeType _type) : syntaxNodeType{_type} {}

tuc::SyntaxNode::SyntaxNode(NodeType _type, const TextEntity& _textValue)
: syntaxNodeType{_type}, textValue{_textValue} {}

/*
constructs a node from a syntax token
*/
tuc::SyntaxNode::SyntaxNode(const Token& _token)
: textValue{_token.text()} {
    switch (_token.type()) {
    case TokenType::TYPE:       syntaxNodeType = NodeType::TYPE; break;
    case TokenType::HASTYPE:    syntaxNodeType = NodeType::HASTYPE; break;
    case TokenType::ASSIGN:     syntaxNodeType = NodeType::ASSIGN; break;
    case TokenType::MAPTO:      syntaxNodeType = NodeType::MAPTO; break;
    case TokenType::ADD:        syntaxNodeType = NodeType::ADD; break;
    case TokenType::SUBTRACT:   syntaxNodeType = NodeType::SUBTRACT; break;
    case TokenType::MULTIPLY:   syntaxNodeType = NodeType::MULTIPLY; break;
    case TokenType::DIVIDE:     syntaxNodeType = NodeType::DIVIDE; break;
    case TokenType::INTEGER:    syntaxNodeType = NodeType::INTEGER; break;
    case TokenType::SEMICOL:    syntaxNodeType = NodeType::SEMICOL; break;
    case TokenType::IDENTIFIER: syntaxNodeType = NodeType::IDENTIFIER; break;
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

tuc::SyntaxNode::NodeType tuc::SyntaxNode::type() const noexcept {
    return syntaxNodeType;
}

bool tuc::SyntaxNode::is_operator() const noexcept {
    return syntaxNodeType == NodeType::ADD || syntaxNodeType == NodeType::SUBTRACT ||
            syntaxNodeType == NodeType::MULTIPLY || syntaxNodeType == NodeType::DIVIDE;
}

std::string tuc::SyntaxNode::value() const noexcept {
    return textValue.text();
}

int tuc::SyntaxNode::position() const noexcept {
    return textValue.position();
}

tuc::TextEntity tuc::SyntaxNode::text() const noexcept {
    return textValue;
}



//~function implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
generate a syntax tree from a list of tokens
*/
std::tuple<std::unique_ptr<tuc::SyntaxNode>, tuc::SymbolTable> tuc::gen_syntax_tree(const std::vector<tuc::Token>& tokenList) {
    auto treeRoot = std::make_unique<tuc::SyntaxNode>(tuc::SyntaxNode::NodeType::PROGRAM);
    auto symTable = SymbolTable{};
    auto nodeStack = std::vector<std::unique_ptr<tuc::SyntaxNode>>{};
    auto opStack = std::vector<tuc::Token>{};

    auto popTokenToNodeStack = [&](){
        auto t = opStack.back();
        opStack.pop_back();
        if (t.is_operator()) {
            auto op = std::make_unique<tuc::SyntaxNode>(t);
            auto n2 = std::move(nodeStack.back());
            nodeStack.pop_back();
            auto n1 = std::move(nodeStack.back());
            nodeStack.pop_back();
            op->append_child(std::move(n1));
            op->append_child(std::move(n2));
            nodeStack.push_back(std::move(op));
        }
        else if (t.type() == tuc::TokenType::HASTYPE) {
            auto op = std::make_unique<tuc::SyntaxNode>(t);
            auto nodeHolder = std::vector<std::unique_ptr<tuc::SyntaxNode>>{};
            while (!nodeStack.empty() && (
                    nodeStack.back()->type() == tuc::SyntaxNode::NodeType::MAPTO ||
                    nodeStack.back()->type() == tuc::SyntaxNode::NodeType::TYPE ||
                    nodeStack.back()->type() == tuc::SyntaxNode::NodeType::IDENTIFIER)) {
                nodeHolder.push_back(std::move(nodeStack.back()));
                nodeStack.pop_back();
            }
            for (int i = nodeHolder.size() - 1; i >= 0; i--) {
                op->append_child(std::move(nodeHolder[i]));
            }
            nodeStack.push_back(std::move(op));
        }
        else if (t.type() == tuc::TokenType::MAPTO) {
            auto op = std::make_unique<tuc::SyntaxNode>(t);
            auto nodeHolder = std::vector<std::unique_ptr<tuc::SyntaxNode>>{};
            while (!nodeStack.empty() && (
                    nodeStack.back()->type() == tuc::SyntaxNode::NodeType::MAPTO ||
                    nodeStack.back()->type() == tuc::SyntaxNode::NodeType::TYPE /*||
                    nodeStack.back()->type() == tuc::SyntaxNode::NodeType::IDENTIFIER*/)) {
                nodeHolder.push_back(std::move(nodeStack.back()));
                nodeStack.pop_back();
            }
            for (int i = nodeHolder.size() - 1; i >= 0; i--) {
                op->append_child(std::move(nodeHolder[i]));
            }
            nodeStack.push_back(std::move(op));
        }
    };

    /*##########################################################################################################
    ### To parse expressions, use a variation of Dijkstra's shunting yard algorithm. Instead of generating a  ##
    ### Reverse Polish Notation (RPN) expression, create syntax trees in equivalent to the expression and     ##
    ### add it to the main syntax tree at the end.                                                            ##
    ##########################################################################################################*/

    for (const auto token: tokenList) {
        if (token.is_operator() /*|| token.type() == tuc::TokenType::IDENTIFIER || token.type() == tuc::TokenType::HASTYPE*/) {
            while(!opStack.empty() /*&& opStack.back().is_operator()*/ && (
                        (token.fixity() == Associativity::LEFT && token.precedence() <= opStack.back().precedence()) ||
                        (token.fixity() == Associativity::RIGHT && token.precedence() < opStack.back().precedence()) )) {
                popTokenToNodeStack();
            }
            opStack.push_back(token);
        }
        else if (token.type() == tuc::TokenType::HASTYPE || token.type() == tuc::TokenType::MAPTO) {
            while(!opStack.empty() /*&& opStack.back().is_operator()*/ && (
                        (token.fixity() == Associativity::LEFT && token.precedence() <= opStack.back().precedence()) ||
                        (token.fixity() == Associativity::RIGHT && token.precedence() < opStack.back().precedence()) )) {
                popTokenToNodeStack();
            }
            opStack.push_back(token);
        }
        else if (token.type() == tuc::TokenType::INTEGER || token.type() == tuc::TokenType::IDENTIFIER || token.type() == tuc::TokenType::TYPE) {
            nodeStack.push_back(std::make_unique<tuc::SyntaxNode>(token));
        }
        else if (token.type() == tuc::TokenType::LPAREN) {
            opStack.push_back(token);
        }
        else if (token.type() == tuc::TokenType::RPAREN) {
            while(opStack.back().type() != tuc::TokenType::LPAREN) {
                popTokenToNodeStack();
            }
            opStack.pop_back();
        }
        else if (token.type() == tuc::TokenType::SEMICOL) {
            while (!opStack.empty()) {
                popTokenToNodeStack();
            }
            treeRoot->append_child(std::move(nodeStack.back()));
            nodeStack.clear();
        }
    }

    return std::make_tuple(std::move(treeRoot), symTable);
}
