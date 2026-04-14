/*
 * OthelloTree.h
 *
 *  Created on: Jun 23, 2024
 *      Author: sandro
 */

#pragma once

#include "tree/Tree.h"
#include "OthelloBoard.h"

namespace othello {

struct Node : public TreeNode<Node> {
	unsigned short m_turn;
	short m_score;
	bool m_black;
	OthelloBoard m_board;
	unsigned long m_pos;
	unsigned short m_nf;

	Node() = delete;
	~Node() = default;
	Node(const Node &other) = default;
	Node(Node &&Node) = default;
	Node& operator=(const Node &other) = default;
	Node& operator=(Node &&other) = default;

	Node (unsigned short turn, unsigned short depth, unsigned long pos, OthelloBoard& board, bool black) :
		TreeNode(depth),
		m_turn(turn),
		m_score(-10000),
		m_black(black),
		m_board(OthelloBoard(board)),
		m_pos(pos),
		m_nf(0)	{
	}

	void eval() override {
		//Do the move
		if (m_pos) {
			m_nf = m_board.flip(m_pos, m_black);
		}
	}

	constexpr bool operator==(const Node& node) const {
		//std::cout << "\n==[" << node << "," << *this << "]==" << std::flush;
		return m_pos == node.m_pos;//m_turn == node.m_turn && m_depth == node.m_depth && m_board == node.m_board;
	}

	std::shared_ptr<Node> getChild(UL pos) {
		auto r = std::find_if(m_children.begin(), m_children.end(),
				[pos](auto pChild){ return pChild->m_pos == pos; });
		return r != std::end(m_children) ? *r : nullptr;
	}


	friend std::ostream& operator<<(std::ostream &f, Node& node) {
		f << std::format(" [D{:d} {:d}-{:s} : {:d}]({:d})", node.m_depth,
				node.m_board.toStandard(node.m_pos), node.m_black ? "Noir" : "Blanc",
						node.m_value,
#ifdef __DEBUG__
						node.m_id
#else
						0L
#endif
						);
		return f;
	}
};

class OthelloTree final : public Tree<Node> {
public:
	OthelloTree(OthelloBoard& board, bool black, unsigned short turn) :
		Tree(),
		m_turn(turn),
		m_black(black) {
			OthelloBoard b = board;
			m_root = std::make_shared<Node>(m_turn, 0, 0L, b, !m_black);
	};
	OthelloTree() = delete;
	~OthelloTree() = default;
	OthelloTree(const OthelloTree &other) : Tree(other) {
		m_black = other.m_black;
		m_turn = other.m_turn;
		OthelloBoard b = other.m_root.get()->m_board;
		m_root = std::make_shared<Node>(m_turn, 0, 0L, b, !m_black);
	};
	OthelloTree(OthelloTree &&other) = default;

	virtual OthelloTree& operator=(const OthelloTree &other) {
		if (this != &other) {
			m_black = other.m_black;
			m_turn = other.m_turn;
			OthelloBoard b = other.m_root.get()->m_board;
			m_root = std::make_shared<Node>(m_turn, 0, 0L, b, !m_black);
		}
		return *this;
	};

//	OthelloTree& operator=(OthelloTree &&other) = default;

//	virtual void buildFrom(std::shared_ptr<Node> pNode) override {
//		Node *node = pNode.get();
//		if (node->m_depth + 1 <= m_maxDepth) {
//			bool black = !node->m_black;
//			OthelloBoard& board = node->m_board;
//			board.board(black).iterateOnPositions([this, &node, &board, &black](unsigned long pos) {
//				if (board.flip(pos, black, true)) {
//					OthelloBoard b = board;
//					std::shared_ptr<Node> child = std::make_shared<Node>(m_turn, node->m_depth + 1, pos, b, black);
//					node->install(child);
//					buildFrom(child);
//				}
//				return true;
//			});
//		}
//	}



protected:
	unsigned short m_turn;
	bool m_black;


};

} /* namespace othello */

