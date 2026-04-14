/*
 * GEvaluationStrategy.cpp
 *
 *  Created on: Jun 14, 2024
 *      Author: sandro
 */

#include "othello/eval/GEvaluationStrategy.h"
#include "util/Util.h"

namespace othello {

void GEvaluationStrategy::eval(Tree<Node>& tree) {
	m_scores.clear();
	Node& root = *(tree.root().get());
	eval(root);

	std::for_each(root.m_children.begin(), root.m_children.end(), [this](std::shared_ptr<Node>& pChild) {
		Node& child = *(pChild.get());
		const US pos = child.m_board.toStandard(child.m_pos);
		m_scores.push_back(std::make_pair(pos, child.m_score));
		//std::cout << std::format("\nE {:d} = {:d}", pos, child.m_score);
	});

	m_result = Util::ramdomBestScoreMove(m_scores);
	//std::cout << std::format("\nRES {:d} = {:d}", m_result.first, m_result.second);
}

void GEvaluationStrategy::eval(Node& node) {
//	const US pos = node.m_board.toStandard(node.m_pos);

//	std::cout << "\n";
//	for(int i{0}; i < node.m_depth - 1; ++i) std::cout << "\t";
//	std::cout << std::format("V {:d}-{:s} D{:d}", pos, node.m_black ? "Noir" : "Blanc", node.m_depth);
//	std::cout << std::flush;

	//doEval(node);
	short sc = 0;
	std::for_each(node.m_children.begin(), node.m_children.end(), [this, &sc](std::shared_ptr<Node>& pChild) {
		Node& child = *(pChild.get());
		eval(child);
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

void GEvaluationStrategy::doEval(Node& node) {
	bool black = node.m_black;
	bool other = !black;
	OthelloBoard& board = node.m_board;
	//UL pos = node.m_pos;
	US turn = node.m_turn;
	US depth = node.m_depth;
	US nf = node.m_nf;

	auto pos = board.toStandard(node.m_pos);
//	std::cout << m_board << std::endl;


	short sc = 1000 - nf +
			m_borderWeight   * (board.border(!black) - board.border(black)) +
			m_mobilityWeight * (board.mobility(black) - board.mobility(!black))
			;

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
			//UL vij = board.toLong(V[i][j]);
			UL vij = VL[i][j];

			//are we playing in X squares (i.e. a neighbor of a corner square) ?
			if (pos == vij) {
				if (board.testflip(V[i][0], other)) {
					sc -= 1000;
//					std::cout << std::format("F{:c}{:d} sc={:d}", (m_piece ? 'B' : 'N'), GPlayer::V[i][0], sc);
				} else {
					sc -= 250;
				}
			}
		}
		//Can we take a corner square?
		if (pos == V[i][0]) {
			sc += 1000;
//			std::cout << std::format(" {:d}=={:d} sc={:d}", cs, m_board.toLong(GPlayer::V[i][0]), sc);
		}
	}

	node.m_value = sc;
//	std::cout << "]" << std::endl;
}


} /* namespace othello */
