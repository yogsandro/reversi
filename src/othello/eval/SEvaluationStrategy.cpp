/*
 * SEvaluationStrategy.cpp
 *
 *  Created on: Jun 28, 2024
 *      Author: sandro
 */

#include "othello/eval/SEvaluationStrategy.h"
#include "util/Util.h"

namespace othello {


void SEvaluationStrategy::eval(Node& node) {
	bool black = node.m_black;
	bool other = !black;
	OthelloBoard& board = node.m_board;
	unsigned short turn = node.m_turn;
	unsigned short depth = node.m_depth;

//	std::cout << m_board << std::endl;


	short sc = //1000 +
			(3 + (turn + depth > 28 ? 1 : 0)) * (board.count(black) - board.count(other)) +
			5   * (board.border(other) - board.border(black)) +
			10 * (board.mobility(black) - board.mobility(other)) +
			500 * (board.corner(black) - board.corner(other))
			;

	node.m_value = sc;
}


} /* namespace othello */
