/*
 * DepthFirstVisitor.h
 *
 *  Created on: Jul 1, 2024
 *      Author: sandro
 */

#ifndef DEPTHFIRSTVISITOR_H_
#define DEPTHFIRSTVISITOR_H_

#include <concepts>

#include "TreeVisitor.h"

namespace othello {

template<typename T>
requires std::derived_from<T, othello::TreeNode<T>>// @suppress("Member declaration not found")
class DepthFirstVisitor : public TreeVisitor<T> {
public:
	DepthFirstVisitor() = default;
	virtual ~DepthFirstVisitor() = default;
	DepthFirstVisitor(const DepthFirstVisitor<T> &other) = default;
	DepthFirstVisitor(DepthFirstVisitor<T> &&other) = default;
	DepthFirstVisitor<T>& operator=(const DepthFirstVisitor<T> &other) = default;
	DepthFirstVisitor<T>& operator=(DepthFirstVisitor<T> &&other) = default;

	virtual void visit(Tree<T>& tree) override {
		T& root = *(tree.root().get());
		visit(root);
	};

	virtual void visit(T& node) override {
		doVisit(node);
		std::for_each(node.m_children.begin(), node.m_children.end(), [this](std::shared_ptr<T>& pChild) {
			T& child = *(pChild.get());
			visit(child);
		});
	};

	virtual void doVisit(T& node) = 0;
};

} /* namespace othello */

#endif /* DEPTHFIRSTVISITOR_H_ */
