/*
Project: TUC
File: syntax_tree.hpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: October 5, 2015

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

#ifndef TUC_SYNTAX_TREE_HPP
#define TUC_SYNTAX_TREE_HPP

// project headers
#include "grammar.hpp"

// standard libraries
#include <memory>
#include <vector>
#include <string>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class SyntaxNode;   // represents a node of a syntax tree
    class SyntaxTree;   // a basic syntax tree to represent analyzed source code
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class tuc::SyntaxNode {
    public:
        enum class NodeType {PROGRAM, ADD, SUBTRACT, MULTIPLY, DIVIDE, INTEGER, LEFTPAREN, RIGHTPAREN, SEMICOL, IDENTIFIER, UNKNOWN};

        SyntaxNode(NodeType _type);

        SyntaxNode(NodeType _type, const std::string& _value, unsigned int _pos);

        SyntaxNode(const Token& _token);
        /*  constructs a node from a syntax token */

        SyntaxNode* parent() noexcept;

        SyntaxNode* child(int i) noexcept;
        /*  returns child with index `i` */

        int child_count() const noexcept;

        void append_child(NodeType _type, const std::string& _value, unsigned int _pos = -1);

        void appedn_child(const Token& _token);

        void append_child(std::unique_ptr<SyntaxNode>&& c) noexcept;
        /*  appends an already existing (allocated) child; since this node must "own" the child, move semantics *must*
            be used to transfer ownership.
        */

        NodeType type() const noexcept;

        bool is_operator() const noexcept;

        std::string value() const noexcept;

        unsigned int position() const noexcept;

    private:
        std::vector<std::unique_ptr<SyntaxNode>> children;
        SyntaxNode* parentNode = nullptr;
        NodeType syntaxNodeType;
        std::string textValue = "";
        unsigned int pos = -1;
};

#endif//TUC_SYNTAX_TREE_HPP
