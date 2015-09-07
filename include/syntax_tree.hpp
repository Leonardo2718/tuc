/*
Project: TUC
File: syntax_tree.hpp
Author: Leonardo Banderali
Created: September 6, 2015
Last Modified: September 6, 2015

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



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    class SyntaxTree;   // a basic syntax tree to represent analyzed source code
}



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
A class that encapsulates the nodes of a tree structure to represent a syntax tree.  The nodes of the tree are
internally stored as pointers to objects that contain the relevant syntax information.  A wrapper class is also provided
for accessing tree nodes from the outside.  This wrapper provides access to the data stored in a node while completely
restricting any possible changes.  The purpose of this wrapper is to abstract away the fact that internally, nodes are
stored as pointers by allowing the user to intercat with nodes as though they were regular objects.  This allows the
user to pass, move, and make copies of these nodes without actually affecting in any way the internal tree nodes.

In order to interact with instances of `SyntaxTree`, users must first obtain the node of interest (Wrapped inside a
`SyntaxNode`) using the getters provided by the instance.  To change a node, and hence the tree, a user has to provide
the wrapped node to the `SyntaxTree` instance methode that will apply the desired change.

By default, the root node holds an empty token.
*/
class tuc::SyntaxTree {
    public:
        class SyntaxNode;           // wrapper class for internal nodes

        SyntaxNode appendChild(SyntaxNode node, const Token& token);
        /*  appends a new child to the current node */

        SyntaxNode preppendChild(SyntaxNode node, const Token& token);
        /*  preppends a new child to the current node */

        int childCount(SyntaxNode node) const;
        /*  returns the number of child nodes */

        SyntaxNode child(SyntaxNode node, int childNumber) const;
        /*  returns a particular child node */

        std::vector<SyntaxNode> children(SyntaxNode node) const;
        /*  returns all child nodes */

        SyntaxNode parent(SyntaxNode node) const;
        /*  returns the parent node */

        SyntaxNode root() const;
        /*  returns the root node of the tree */

    private:
        class InternalSyntaxNode;   // class representing internal nodes

        std::unique_ptr<InternalSyntaxNode> rootNode = std::make_unique<InternalSyntaxNode>();
};

/*
Wrapper class for internal tree nodes.  Internally, it stores a pointer to the tree node it is linked to.  Since it is
only a wrapper class, it DOES NOT participate in the ownership of the node.  So, the default, shallow operations are
okay (copy construct, copy assignment, destructor, etc...).
*/
class tuc::SyntaxTree::SyntaxNode {
    public:
        SyntaxNode() = default;

        Token token() const;

    private:
        explicit SyntaxNode(InternalSyntaxNode* _internalNode);

        InternalSyntaxNode* internalNode = nullptr; // points to internal tree node but DOES NOT own it

        friend SyntaxTree;
};

/*
Class representing internal tree nodes.
*/
class tuc::SyntaxTree::InternalSyntaxNode {
    public:
        InternalSyntaxNode(const Token& _t = Token{}, InternalSyntaxNode* _parent = nullptr);

        Token token;
        InternalSyntaxNode* parent = nullptr;
        std::vector<std::unique_ptr<InternalSyntaxNode>> children;
};

#endif//TUC_SYNTAX_TREE_HPP
