/*
 * Tree.h
 *
 *  Created on: Jun 2, 2024
 *      Author: sandro
 */

#pragma once

#include <concepts>
#include <atomic>
#include <format>
#include <vector>
#include <memory>
#include <algorithm>
#include "TreeVisitor.h"

namespace othello {

//struct TreeNode;

template<typename T>
//requires std::derived_from<T, othello::TreeNode<T>>
struct TreeNode {
public :
#ifdef __DEBUG__
	unsigned long m_id;
#endif
	unsigned short m_depth;
	short m_value;
	std::vector<std::shared_ptr<T>> m_children;
	std::shared_ptr<T> m_parent;

	TreeNode() = delete;
	virtual ~TreeNode() = default;
	TreeNode(const TreeNode &other) = default;
	TreeNode(TreeNode &&other) = default;
	TreeNode& operator=(const TreeNode &other) = default;
	TreeNode& operator=(TreeNode &&other) = default;

	TreeNode (unsigned short depth, short value = 0) :
		TreeNode(nullptr, depth, value) {
	}

	TreeNode (std::shared_ptr<T> pParent, unsigned short depth, short value = 0) :
#ifdef __DEBUG__
		m_id(TreeNode::C++),
#endif
		m_depth(depth),
		m_value(value),
		m_parent(pParent) {
	}

	void install(const std::shared_ptr<T> child) {
		m_children.push_back(child);
		child.get()->eval();
	}

	bool hasChild() const {
		return m_children.size() > 0;
	}

	unsigned short nbChildren() const {
		return m_children.size();
	}

	void addChild(std::shared_ptr<T> child) {
		child.get()->m_depth = m_depth + 1;
		m_children.push_back(child);
	}

	std::shared_ptr<T> getChild(const std::shared_ptr<T>& child) {
		return getChild(*child);
	}

	std::shared_ptr<T> getChild(const T& node) {
		auto r = std::find_if(m_children.begin(), m_children.end(),
				[&node](std::shared_ptr<T> pChild){ return *pChild == node; });
		return r != std::end(m_children) ? *r : nullptr;
	}

	const std::vector<std::shared_ptr<T>> leafs() const {
		std::vector<std::shared_ptr<T>> leafs_;
		if (hasChild()) {
			std::for_each(m_children.begin(), m_children.end(), [&leafs_](auto pChild) {
				auto cl = pChild->leafs();
				if (!cl.empty()) {
					std::for_each(cl.begin(), cl.end(), [&leafs_](auto pc) {
						leafs_.push_back(pc);
					});
				} else {
					leafs_.push_back(pChild);
				}
			});
		}
		return leafs_;
	}

	void setDepth(unsigned short depth) {
		m_depth = depth;
		std::for_each(m_children.begin(), m_children.end(), [depth](auto pChild) {
			pChild->setDepth(depth + 1);
		});
	}

	virtual void eval() = 0;

	virtual long hash() const {
#ifdef __DEBUG__
		return m_id;
#else
		return 0L;
#endif
	}

	static std::atomic_ulong C;
};

template<typename T>
std::atomic_ulong TreeNode<T>::C = 0L;

template<typename T>
requires std::derived_from<T, othello::TreeNode<T>>// @suppress("Member declaration not found")
class TreeVisitor;

template<typename T>
//requires std::derived_from<T, othello::TreeNode<T>>
class Tree {

public:
	Tree(std::shared_ptr<T> root = nullptr) :
		m_root(root) {
	};
	virtual ~Tree() = default;
//	Tree(const Tree<T>& other) = default;
	Tree(Tree<T> &&tree) = default;
//	Tree<T>& operator=(const Tree<T>& other) = default;
	Tree<T>& operator=(Tree<T> &&other) = default;

	Tree(const Tree &other) {
		T t = *(other.m_root.get());
		m_root = std::make_shared<T>(t);
	};

//	virtual Tree& operator=(const Tree &other) {
//		if (this != &other) {
//			T t = *(other.m_root.get());
//			m_root = std::make_shared<T>(t);
//		}
//		return *this;
//	};

//	virtual void buildFrom(std::shared_ptr<T> pNode) = 0 ;

	const std::shared_ptr<T> root() const {
		return m_root;
	}

	const std::vector<std::shared_ptr<T>> leafs() const {
		return m_root->hasChild() ? m_root->leafs() : std::vector<std::shared_ptr<T>>({m_root});
	}

	void setRoot(std::shared_ptr<T> root) {
		m_root = root;
		m_root->setDepth(0);
	}

	unsigned short depth() const {
		return 0;
	}

	void accept(TreeVisitor<T>& visitor){
		visitor.visit(*this);
	}

	/**
	 *
	 * @param f
	 * @param node
	 */
	void printFrom(std::ostream &f, const std::shared_ptr<T>& pNode) {
		//f << "\n";//========================================================================\n";
		T& node = *(pNode.get());
		for(int i{0}; i < node.m_depth - 1; ++i) f << "\t";
		f << node << "\n";
		std::for_each(node.m_children.begin(), node.m_children.end(), [this, &f](std::shared_ptr<T>& child) {
			printFrom(f, child);
		});
		//f << std::endl;
	}

	friend std::ostream& operator<<(std::ostream &f, Tree<T>& tree) {
		tree.printFrom(f, tree.root());
		return f;
	}

//	unsigned short maxDepth() const {
//		return m_maxDepth;
//	}

protected:

	std::shared_ptr<T> m_root;

};

} /* namespace othello */

