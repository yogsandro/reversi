/*
 * TreeVisitor.h
 *
 *  Created on: Jun 10, 2024
 *      Author: sandro
 */

#ifndef TREEVISITOR_H_
#define TREEVISITOR_H_

#include <concepts>

#include "Tree.h"

namespace othello {

template<typename T>
class Tree;
template<typename T>
class TreeNode;

/**
 * This class is a Tree visitor
 */
template<typename T>
requires std::derived_from<T, othello::TreeNode<T>>// @suppress("Member declaration not found")
class TreeVisitor {
public:
	TreeVisitor() = default;
	virtual ~TreeVisitor() = default;
	TreeVisitor(const TreeVisitor<T> &other) = default;
	TreeVisitor(TreeVisitor<T> &&other) = default;
	TreeVisitor<T>& operator=(const TreeVisitor<T> &other) = default;
	TreeVisitor<T>& operator=(TreeVisitor<T> &&other) = default;

	virtual void visit(Tree<T>& tree) = 0;
	virtual void visit(T& node) = 0;

};

} /* namespace othello */

#endif /* TREEVISITOR_H_ */
