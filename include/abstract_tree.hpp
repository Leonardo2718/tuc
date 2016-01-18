/*
Project: TUC
File: abstract_tree.hpp
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

#ifndef TUC_ABSTRACT_TREE_HPP
#define TUC_ABSTRACT_TREE_HPP

// standard libraries
#include <memory>
#include <vector>
#include <iostream>



//~declare namespace members~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

namespace tuc {
    template <typename SubType> class AbstractTreeNode; // an abstract, templated (CRTP) tree node type
}



//~overloaded functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename SubType>
std::ostream& operator<< (std::ostream& os, const tuc::AbstractTreeNode<SubType>* node);
/*  puts a textual representation of a node hierarchy in an output stream */



//~class declarations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

template <typename SubType>
class tuc::AbstractTreeNode {
    public:
        auto parent() const noexcept -> const SubType*;
        /*  returns the parent node */

        auto parent() noexcept -> SubType*;

        auto child(int i) const noexcept -> const SubType*;
        /*  returns the child with index `i` */

        auto child(int i) noexcept -> SubType*;

        auto child_count() const noexcept -> int;
        /*  returns the number of children */

        void append_child(std::unique_ptr<SubType>&& c) noexcept;
        /*  appends an already existing (allocated) child; since this node must "own" the child, move semantics *must*
            be used to transfer ownership.
        */

        template <typename... Args>
        void append_child(Args&&... args);
        /*  constructs a new child with the arguments given and appends it */

        auto remove(int i) -> std::unique_ptr<SubType>;
        /*  removes and returns child with index `i` */

    private:
        std::vector<std::unique_ptr<SubType>> children;
        SubType* parentNode = nullptr;
};



//~class implementations~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
returns the parent node
*/
template <typename SubType>
auto tuc::AbstractTreeNode<SubType>::parent() const noexcept -> const SubType* {
    return parentNode;
}

template <typename SubType>
auto tuc::AbstractTreeNode<SubType>::parent() noexcept -> SubType* {
    return parentNode;
}

/*
returns the child with index `i`
*/
template <typename SubType>
auto tuc::AbstractTreeNode<SubType>::child(int i) const noexcept -> const SubType* {
    return children[i].get();
}

template <typename SubType>
auto tuc::AbstractTreeNode<SubType>::child(int i) noexcept -> SubType* {
    return children[i].get();
}

/*
returns the number of children
*/
template <typename SubType>
auto tuc::AbstractTreeNode<SubType>::child_count() const noexcept -> int {
    return children.size();
}

/*
appends an already existing (allocated) child; since this node must "own" the child, move semantics *must*
be used to transfer ownership.
*/
template <typename SubType>
void tuc::AbstractTreeNode<SubType>::append_child(std::unique_ptr<SubType>&& c) noexcept {
    children.push_back(std::forward<std::unique_ptr<SubType>>(c));
}

/*
constructs a new child with the arguments given and appends it
*/
template <typename SubType>
template <typename... Args>
void tuc::AbstractTreeNode<SubType>::append_child(Args&&... args) {
    children.push_back(std::make_unique<SubType>(std::forward<Args>(args)...));
}

/*
removes and returns child with index `i`
*/
template <typename SubType>
auto tuc::AbstractTreeNode<SubType>::remove(int i) -> std::unique_ptr<SubType> {}



//~overloaded functions~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

/*
puts a textual representation of a node hierarchy in an output stream
*/
template <typename SubType>
std::ostream& operator<< (std::ostream& os, const tuc::AbstractTreeNode<SubType>* node) {
    using NodeStack = std::vector<const tuc::AbstractTreeNode<SubType>*>;
    using NodeStackSize = typename NodeStack::size_type;
    auto nodeStack = NodeStack{};
    auto childIndexStack = std::vector<int>{};

    os << *static_cast<const SubType*>(node) << "\n";

    nodeStack.push_back(node);
    childIndexStack.push_back(0);
    while (!nodeStack.empty()) {
        if (childIndexStack.back() < nodeStack.back()->child_count()) {
            auto child = nodeStack.back()->child(childIndexStack.back());

            for (NodeStackSize i = 0, s = nodeStack.size() - 1; i < s; i++) {
                if (childIndexStack[i] < nodeStack[i]->child_count())
                    os << " |   ";
                else
                    os << "     ";
            }

            os << " |-> " << *child << "\n";

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
                for (NodeStackSize i = 0, s = nodeStack.size(); i < s; i++) {
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

#endif//TUC_ABSTRACT_TREE_HPP
