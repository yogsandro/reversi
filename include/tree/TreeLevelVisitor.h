/*
 * TreeLevelVisitor.h
 *
 *  Created on: Jul 1, 2024
 *      Author: sandro
 */

#ifndef TREELEVELVISITOR_H_
#define TREELEVELVISITOR_H_

#include <concepts>
#include <vector>

#include "DepthFirstVisitor.h"

namespace othello {

template<typename T>
requires std::derived_from<T, othello::TreeNode<T>>// @suppress("Member declaration not found")
class TreeLevelVisitor : public DepthFirstVisitor<T> {
public:
	TreeLevelVisitor() = delete;
	TreeLevelVisitor(unsigned short depth) :
		DepthFirstVisitor<T>(),
		m_depth(depth) {
	};
	virtual ~TreeLevelVisitor() = default;
	TreeLevelVisitor(const TreeLevelVisitor<T> &other) = default;
	TreeLevelVisitor(TreeLevelVisitor<T> &&other) = default;
	TreeLevelVisitor<T>& operator=(const TreeLevelVisitor<T> &other) = default;
	TreeLevelVisitor<T>& operator=(TreeLevelVisitor<T> &&other) = default;

	virtual void doVisit(T& node) override {
		if (node.m_depth == m_depth) {
			m_nodes.push_back(std::ref(node));
		}
	}

	std::vector<std::reference_wrapper<T>>& getNodes() {
		return m_nodes;
	}

protected:
	unsigned short m_depth;
	std::vector<std::reference_wrapper<T>> m_nodes;
};

} /* namespace othello */

#endif /* TREELEVELVISITOR_H_ */
