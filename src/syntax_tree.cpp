/*
Project: TUC
File: syntax_tree.cpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: October 9, 2015

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

// standard libraries
#include <stdexcept>
#include <sstream>
#include <iostream>



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
    case TokenType::ASSIGN:     syntaxNodeType = NodeType::ASSIGN; break;
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



//~function implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
generate a syntax tree from a list of tokens
*/
std::tuple<std::unique_ptr<tuc::SyntaxNode>, tuc::SymbolTable> tuc::gen_syntax_tree(const std::vector<tuc::Token>& tokenList) {
    auto treeRoot = std::make_unique<tuc::SyntaxNode>(tuc::SyntaxNode::NodeType::PROGRAM);
    auto symTable = SymbolTable{};
    auto rpnExpr = std::vector<tuc::Token>{};
    auto opStack = std::vector<tuc::Token>{};

    /*##########################################################################################################
    ### To parse mathematical expressions, first use a variation of Dijkstra's shunting yard algorithm to     ##
    ### transform the expression into Reverse Polish Notation (RPN).  Once transformed, the expression can be ##
    ### read *right-to-left* (*backwards*) into the syntax tree.  This is because the last operations to be   ##
    ### executed are the ones that must appear at the top of the syntax tree for the expression.  This also   ##
    ### means that the branches (linking to the operator's operands) must also be added right-to-left.        ##
    ###                                                                                                       ##
    ### This top-down approach is neither efficient nor elegant.  So, it will likely be replaced by a         ##
    ### bottom-up parser in the future.                                                                       ##
    ##########################################################################################################*/

    for (const auto token: tokenList) {
        if (token.is_operator() /*|| token.type() == tuc::TokenType::IDENTIFIER*/) {
            while(!opStack.empty() && opStack.back().is_operator() && (
                        (token.fixity() == Associativity::LEFT && token.precedence() <= opStack.back().precedence()) ||
                        (token.fixity() == Associativity::RIGHT && token.precedence() < opStack.back().precedence()) )) {
                rpnExpr.push_back(opStack.back());
                opStack.pop_back();
            }
            opStack.push_back(token);
        }
        else if (token.type() == tuc::TokenType::IDENTIFIER) {
            auto symIterator = symTable.find(token.lexeme());
            if (symIterator != symTable.end()) {   // test if symbol is defined in the symbol table
                // treat symbol as an operator
                while(!opStack.empty() && opStack.back().is_operator() && (
                            (token.fixity() == Associativity::LEFT && token.precedence() <= opStack.back().precedence()) ||
                            (token.fixity() == Associativity::RIGHT && token.precedence() < opStack.back().precedence()) )) {
                    rpnExpr.push_back(opStack.back());
                    opStack.pop_back();
                }
                opStack.push_back(token);
            }
            else {
                // if symbol is not defined in the symbol table, throw a runtime exception
                //%TODO% create specialized exceptions for compilation errors %%
                auto err = std::stringstream{};
                err << "Undefined reference to symbol `" << token.lexeme() << "`.\n";
                throw std::runtime_error{err.str()};
            }
        }
        else if (token.type() == tuc::TokenType::INTEGER) {
            rpnExpr.push_back(token);
        }
        else if (token.type() == tuc::TokenType::LPAREN) {
            opStack.push_back(token);
        }
        else if (token.type() == tuc::TokenType::RPAREN) {
            auto t = opStack.back();
            while(t.type() != tuc::TokenType::LPAREN) {
                rpnExpr.push_back(t);
                opStack.pop_back();
                t = opStack.back();
            }
        }
        else if (token.type() == tuc::TokenType::SEMICOL) {
            while (!opStack.empty()) {
                rpnExpr.push_back(opStack.back());
                opStack.pop_back();
            }
            auto nodeStack = std::vector<std::unique_ptr<tuc::SyntaxNode>>{};
            for (auto t : rpnExpr) {
                auto node = std::make_unique<tuc::SyntaxNode>(t);
                if (node->is_operator()) {
                    auto secondVal = std::move(nodeStack.back());   // using `std::move()` causes `.back()` to exist
                    nodeStack.pop_back();                           // also note that node come out in reverse order
                    auto firstVal = std::move(nodeStack.back());
                    nodeStack.pop_back();
                    node->append_child(std::move(firstVal));
                    node->append_child(std::move(secondVal));
                    nodeStack.push_back(std::move(node));
                } else if (t.type() == tuc::TokenType::INTEGER) {
                    nodeStack.push_back(std::move(node));
                }
            }
            treeRoot->append_child(std::move(nodeStack.back()));
            rpnExpr.clear();
        }
    }

    return std::make_tuple(std::move(treeRoot), symTable);
}
