/*
 * GEvalStrategy.cpp
 *
 *  Created on: Jun 10, 2024
 *      Author: sandro
 */

#include <format>
#include <vector>

#include "othello/OthelloTree.h"
#include "othello/eval/EvalStrategy.h"
#include "util/Util.h"

namespace othello {


void EvalStrategy::visit(Tree<Node>& tree) {
	m_scores.clear();
	Node& root = *(tree.root().get());
	visit(root);

	std::for_each(root.m_children.begin(), root.m_children.end(), [this](std::shared_ptr<Node>& pChild) {
		Node& child = *(pChild.get());
		const US pos = child.m_board.toStandard(child.m_pos);
		m_scores.push_back(std::make_pair(pos, child.m_score));
		//std::cout << std::format("\nE {:d} = {:d}", pos, child.m_score);
	});

	m_result = Util::ramdomBestScoreMove(m_scores);
	//std::cout << std::format("\nRES {:d} = {:d}", m_result.first, m_result.second);
}

void EvalStrategy::visit(Node& node) {
//	const US pos = node.m_board.toStandard(node.m_pos);

//	std::cout << "\n";
//	for(int i{0}; i < node.m_depth - 1; ++i) std::cout << "\t";
//	std::cout << std::format("V {:d}-{:s} D{:d}", pos, node.m_black ? "Noir" : "Blanc", node.m_depth);
//	std::cout << std::flush;

	eval(node);
	short sc = 0;
	std::for_each(node.m_children.begin(), node.m_children.end(), [this, &sc](std::shared_ptr<Node>& pChild) {
		Node& child = *(pChild.get());
		visit(child);
		if (child.m_value > sc) {
			sc = child.m_value;
		}
	});

	node.m_score = node.m_value + sc * (1 - 2 * (node.m_depth % 2));
//	if (node.m_depth == 1) {
//		//const US pos = node.m_board.toStandard(node.m_pos);
//		m_scores.push_back(std::make_pair(pos, score));
//		std::cout << std::format("\nE {:d} = {:d}", pos, score);
//	}

}

void EvalStrategy::eval(Node& node) const {
	bool black = node.m_black;
	bool other = !black;
	OthelloBoard& board = node.m_board;
//	UL pos = node.m_pos;
	US turn = node.m_turn;
	US depth = node.m_depth;
	US nf = node.m_nf;
	const US pos = node.m_board.toStandard(node.m_pos);


//	//Do the move
//	short nf = board.flip(pos, black, false);
//	std::cout << m_board << std::endl;

	short sc = 1000 - nf +
			m_borderWeight   * (board.border(other) - board.border(black)) +
			m_mobilityWeight * (board.mobility(black) - board.mobility(other));

//	short sc = 1000 - nf +
//			m_borderWeight   * (board.border(other) - board.border(black)) +
//			m_mobilityWeight * (board.mobility(black) - board.mobility(other));

//	for (int i {0}; i < m_depth - 1; ++i) {
//		std::cout << "\t";
//	}
//	std::cout << std::format("E[{:d} sc={:d}", m_board.toStandard(cs), sc) ;
	if (turn + depth > 28) {
		sc += 2 * nf;
//		std::cout << std::format(" sc={:d}", sc);
	}

	for (int i {0}; i < 4; ++i) {
		for (int j {1}; j < 4; ++j) {
			UL vij = board.toLong(V[i][j]);
			//are we playing in X squares (i.e. a neighbor of a corner square) ?
			if (pos == vij) {
				if (board.testflip(vij, other)) {
					sc -= 1000;
//					std::cout << std::format("F{:c}{:d} sc={:d}", (m_piece ? 'B' : 'N'), GPlayer::V[i][0], sc);
				} else {
					sc -= 250;
				}
			}
		}
		//Can we take a corner square?
		if (pos == V[i][0]) {
			sc += 10000;
//			std::cout << std::format(" {:d}=={:d} sc={:d}", cs, m_board.toLong(GPlayer::V[i][0]), sc);
		}
	}

	node.m_value = sc;
//	std::cout << "]" << std::endl;
}



} /* namespace othello */
