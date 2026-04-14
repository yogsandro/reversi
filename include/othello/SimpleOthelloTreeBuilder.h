/*
 * SimpleOthelloTreeBuilder.h
 *
 *  Created on: Jun 30, 2024
 *      Author: sandro
 */

#pragma once
#include "builder/TreeBuilder.h"
#include "OthelloTree.h"

namespace othello {

class SimpleOthelloTreeBuilder final : public TreeBuilder<Node> {
public:
	SimpleOthelloTreeBuilder(OthelloTree& tree, US maxDepth) :
		TreeBuilder<Node>(tree, maxDepth) {
	};
	SimpleOthelloTreeBuilder() = delete;
	~SimpleOthelloTreeBuilder() = default;
	SimpleOthelloTreeBuilder(const SimpleOthelloTreeBuilder &other) = default;
	SimpleOthelloTreeBuilder(SimpleOthelloTreeBuilder &&other) = default;
	SimpleOthelloTreeBuilder& operator=(const SimpleOthelloTreeBuilder &other) = default;
	SimpleOthelloTreeBuilder& operator=(SimpleOthelloTreeBuilder &&other) = default;

//	Tree<Node>& build() override {
	void build() override {
		buildFrom(m_tree.root());
		//return m_tree;
	}

	void buildFrom(std::shared_ptr<Node> pNode) {
		Node *node = pNode.get();
		if (node->m_depth + 1 <= m_maxTreeDepth) {
			bool black = !node->m_black;
			OthelloBoard& board = node->m_board;
			board.board(black).iterateOnPositions([this, &node, &board, &black](UL pos) {
				if (board.testflip(pos, black)) {
					OthelloBoard b = board;
					std::shared_ptr<Node> child = std::make_shared<Node>(node->m_turn, node->m_depth + 1, pos, b, black);
					node->install(child);
					buildFrom(child);
				}
				return true;
			});
		}
	}

};

} /* namespace othello */

