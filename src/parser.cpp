/*
Project: TUC
File: parser.cpp
Author: Leonardo Banderali
Created: January 17, 2016
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
#include "parser.hpp"
#include "compiler_exceptions.hpp"



//~function implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
generate a syntax tree from a list of tokens
*/
std::unique_ptr<tuc::SyntaxNode> tuc::gen_syntax_tree(const std::vector<tuc::Token>& tokenList) {
    auto treeRoot = std::make_unique<tuc::SyntaxNode>(tuc::NodeType::PROGRAM);
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
        if (is_exp_entity(token.type())) {
            if (tempValueExpression) {
                auto newNode = std::make_unique<tuc::SyntaxNode>(token);
                newNode->append_child(std::move(tempValueExpression));
                tempValueExpression = std::move(newNode);
            }
            else
                tempValueExpression = std::make_unique<tuc::SyntaxNode>(token);
        }
        else if (is_highorder_op(token.type())) {
            if (tempValueExpression)
                nodeStack.push_back(std::move(tempValueExpression));
            while(!operatorStack.empty() && (
                        (token.fixity() == Associativity::LEFT && token.precedence() <= operatorStack.back().precedence()) ||
                        (token.fixity() == Associativity::RIGHT && token.precedence() < operatorStack.back().precedence()) )) {
                popTokenToNodeStack();
            }
            operatorStack.push_back(token);
        }
        else if (token.type() == tuc::NodeType::LPAREN) {
            operatorStack.push_back(token);
        }
        else if (token.type() == tuc::NodeType::RPAREN) {
            if (tempValueExpression)
                nodeStack.push_back(std::move(tempValueExpression));
            while(operatorStack.back().type() != tuc::NodeType::LPAREN) {
                popTokenToNodeStack();
                if (operatorStack.empty())
                    throw tuc::CompilerException::MismatchedParenthesis{token.text()};
            }
            operatorStack.pop_back();
        }
        else if (token.type() == tuc::NodeType::SEMICOL) {
            if (tempValueExpression)
                nodeStack.push_back(std::move(tempValueExpression));
            while (!operatorStack.empty()) {
                if (operatorStack.back().type() == tuc::NodeType::LPAREN)
                    throw tuc::CompilerException::MismatchedParenthesis{operatorStack.back().text()};
                popTokenToNodeStack();
            }
            treeRoot->append_child(std::move(nodeStack.back()));
            nodeStack.clear();
        }
    }

    return treeRoot;
}
