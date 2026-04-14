/*
 * TreeBuilder.h
 *
 *  Created on: Jun 30, 2024
 *      Author: sandro
 */

#pragma once

#include "../tree/Tree.h"
#include "../Config.h"

namespace othello {


template <typename T>
class TreeBuilder {
public:

	TreeBuilder() = delete;
	TreeBuilder(Tree<T>& tree, unsigned short maxTreeDepth) :
		m_tree(tree),
		m_maxTreeDepth(maxTreeDepth),
		verbose (Config::VERBOSE) {
	};
	virtual ~TreeBuilder() = default;
	TreeBuilder(const TreeBuilder<T> &other) = default;
	TreeBuilder(TreeBuilder<T> &&other) = default;
	TreeBuilder<T>& operator=(const TreeBuilder<T> &other) = default;
	TreeBuilder<T>& operator=(TreeBuilder<T> &&other) = default;

//	virtual Tree<T>& build() = 0;
	virtual void build() = 0;

	unsigned short getMaxTreeDepth() const {
		return m_maxTreeDepth;
	}

	void setMaxTreeDepth(unsigned short maxTreeDepth) {
		m_maxTreeDepth = maxTreeDepth;
	}

	void setTree(Tree<T>& tree) {
		m_tree = tree;
	}

	Tree<T>& getTree() {
		return m_tree;
	}

	virtual Tree<T> getTreeCopy() {
		return m_tree;
	}



protected :

	/**
	 * The result tree
	 */
	Tree<T>& m_tree;

	/**
	 * Max depth of the tree
	 */
	unsigned short m_maxTreeDepth;



public:
	bool verbose;
};

} /* namespace othello */

