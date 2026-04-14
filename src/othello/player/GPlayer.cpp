/*
 * GPlayer.cpp
 *
 *  Created on: Jun 2, 2024
 *      Author: sandro
 */


#include <format>

#include "othello/player/GPlayer.h"
#include "othello/OthelloTree.h"
#include "othello/SimpleOthelloTreeBuilder.h"
#include "util/Util.h"
#include "othello/eval/GEvaluationStrategy.h"
#include "othello/OthelloParallelTreeBuilder.h"
namespace othello {


US GPlayer::play() {
	bool parallel = true;
	auto& board = m_pGameRegistry->board();
	bool blacksTurn = m_pGameRegistry->color(*this);
	short t = m_pGameRegistry->turn();

	US move = 0;

	if (board.hasMove(blacksTurn)) {
		long count = 0;
		if (parallel) {
			OthelloParallelTreeBuilder<Node> builder(board, blacksTurn, t, m_treeStrategy, m_treeStrategy.maxDepth(), 100);
			Tree<Node>& ptree = builder.build();
			count = builder.count;
			m_treeStrategy.eval(ptree);
		} else {
			//we build a tree of all possible moves
			OthelloTree tree(board, blacksTurn, t);
			SimpleOthelloTreeBuilder builder = SimpleOthelloTreeBuilder(tree, m_treeStrategy.maxDepth());
			builder.build();
			m_treeStrategy.eval(tree);
		}

		const std::pair<US, short>& result = m_treeStrategy.getResult();
		move = result.first;

		if (!m_silent) {
			std::cout << std::format("[{:s}] Je joue en {:s} ({:d})", m_name, Util::string(move), count);
			std::cout << std::endl;
		}

		board.play(move, blacksTurn);
	} else {
		if (!m_silent) std::cout << "[G] Je dois passer !\n";
	}
	return move;
}


} /* namespace othello */
