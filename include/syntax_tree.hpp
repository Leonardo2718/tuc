/*
Project: TUC
File: syntax_tree.hpp
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

#ifndef TUC_SYNTAX_TREE_HPP
#define TUC_SYNTAX_TREE_HPP

// project headers
#include "grammar.hpp"
#include "text_entity.hpp"
#include "symbol_table.hpp"

// standard libraries
#include <tuple>
#include <memory>
#include <vector>
#include <string>
#include <iostream>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class SyntaxNode;   // represents a node of a syntax tree

    // generate a syntax tree and symbol table from a list of tokens
    std::tuple<std::unique_ptr<SyntaxNode>, SymbolTable> gen_syntax_tree(const std::vector<Token>& tokenList);
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class tuc::SyntaxNode {
    public:
        enum class NodeType {PROGRAM, TYPE, HASTYPE, ASSIGN, MAPTO, ADD, SUBTRACT, MULTIPLY, DIVIDE, INTEGER, LEFTPAREN, RIGHTPAREN, SEMICOL,
                                IDENTIFIER, UNKNOWN};

        SyntaxNode(NodeType _type);

        SyntaxNode(NodeType _type, const TextEntity& _textValue);

        explicit SyntaxNode(const Token& _token);
        /*  constructs a node from a syntax token */

        const SyntaxNode* parent() const noexcept;

        SyntaxNode* parent() noexcept;

        const SyntaxNode* child(int i) const noexcept;
        /*  returns child with index `i` */

        SyntaxNode* child(int i) noexcept;

        int child_count() const noexcept;

        void append_child(NodeType _type, const TextEntity& _textValue);

        void append_child(const Token& _token);

        void append_child(std::unique_ptr<SyntaxNode>&& c) noexcept;
        /*  appends an already existing (allocated) child; since this node must "own" the child, move semantics *must*
            be used to transfer ownership.
        */

        NodeType type() const noexcept;

        bool is_operator() const noexcept;

        std::string value() const noexcept;

        TextEntity text() const noexcept;

        int position() const noexcept;

    private:
        std::vector<std::unique_ptr<SyntaxNode>> children;
        SyntaxNode* parentNode = nullptr;
        NodeType syntaxNodeType;
        TextEntity textValue;
};



//~overloaded functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

std::ostream& operator<< (std::ostream& os, const tuc::SyntaxNode* node);
/*  puts a textual representation of a node hierarchy in an output stream */

std::ostream& operator<< (std::ostream& os, const std::unique_ptr<tuc::SyntaxNode, std::default_delete<tuc::SyntaxNode>>& node);
/*  puts a textual representation of a node hierarchy in an output stream */

#endif//TUC_SYNTAX_TREE_HPP
