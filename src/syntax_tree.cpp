/*
Project: TUC
File: syntax_tree.cpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: January 6, 2016

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
#include "compiler_exceptions.hpp"



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



//~function implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
generate a syntax tree from a list of tokens
*/
std::tuple<std::unique_ptr<tuc::SyntaxNode>, tuc::SymbolTable> tuc::gen_syntax_tree(const std::vector<tuc::Token>& tokenList) {
    auto treeRoot = std::make_unique<tuc::SyntaxNode>(tuc::SyntaxNode::NodeType::PROGRAM);
    auto symTable = SymbolTable{};
    auto nodeStack = std::vector<std::unique_ptr<tuc::SyntaxNode>>{};
    auto operatorStack = std::vector<tuc::Token>{};
    auto tempValueExpression = std::unique_ptr<tuc::SyntaxNode>{};  // a temporary node for a value expression
                                                                    // (combination of literals, types, and identifiers)

    auto popTokenToNodeStack = [&](){
        auto t = operatorStack.back();
        operatorStack.pop_back();
        auto op = std::make_unique<tuc::SyntaxNode>(t);
        auto n2 = std::move(nodeStack.back());
        nodeStack.pop_back();
        auto n1 = std::move(nodeStack.back());
        nodeStack.pop_back();
        op->append_child(std::move(n1));
        op->append_child(std::move(n2));
        nodeStack.push_back(std::move(op));
    };

    for (const auto& token: tokenList) {
        if (token.type() == tuc::TokenType::INTEGER || token.type() == tuc::TokenType::IDENTIFIER || token.type() == tuc::TokenType::TYPE) {
            if (tempValueExpression) {
                auto newNode = std::make_unique<tuc::SyntaxNode>(token);
                newNode->append_child(std::move(tempValueExpression));
                tempValueExpression = std::move(newNode);
            }
            else
                tempValueExpression = std::make_unique<tuc::SyntaxNode>(token);
        }
        else if (token.is_operator() || token.type() == tuc::TokenType::HASTYPE || token.type() == tuc::TokenType::MAPTO) {
            if (tempValueExpression)
                nodeStack.push_back(std::move(tempValueExpression));
            while(!operatorStack.empty() && (
                        (token.fixity() == Associativity::LEFT && token.precedence() <= operatorStack.back().precedence()) ||
                        (token.fixity() == Associativity::RIGHT && token.precedence() < operatorStack.back().precedence()) )) {
                popTokenToNodeStack();
            }
            operatorStack.push_back(token);
        }
        else if (token.type() == tuc::TokenType::LPAREN) {
            operatorStack.push_back(token);
        }
        else if (token.type() == tuc::TokenType::RPAREN) {
            if (tempValueExpression)
                nodeStack.push_back(std::move(tempValueExpression));
            while(operatorStack.back().type() != tuc::TokenType::LPAREN) {
                popTokenToNodeStack();
                if (operatorStack.empty())
                    throw tuc::CompilerException::MismatchedParenthesis{token.text()};
            }
            operatorStack.pop_back();
        }
        else if (token.type() == tuc::TokenType::SEMICOL) {
            if (tempValueExpression)
                nodeStack.push_back(std::move(tempValueExpression));
            while (!operatorStack.empty()) {
                if (operatorStack.back().type() == tuc::TokenType::LPAREN)
                    throw tuc::CompilerException::MismatchedParenthesis{operatorStack.back().text()};
                popTokenToNodeStack();
            }
            treeRoot->append_child(std::move(nodeStack.back()));
            nodeStack.clear();
        }
    }

    return std::make_tuple(std::move(treeRoot), symTable);
}
