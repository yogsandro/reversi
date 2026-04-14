/*
 * IDPlayer.h
 *
 *  Created on: Jul 0, 2024
 *      Author: sandro
 */

#pragma once

#include "othello/eval/SEvaluationStrategy.h"
#include "othello/OthelloBoard.h"
#include "OthelloPlayer.h"
#include "builder/MTDf.h"
#include "builder/IterativeDeepening.h"
#include "othello/OthelloAlphaBetaTreeBuilder.h"


namespace othello {

/**
 * Player following the MTD(f) algorithm
 */
class MTDfPlayer final : public OthelloPlayer {
public:
	MTDfPlayer() = delete;
	MTDfPlayer(std::string name, std::string desc, unsigned short maxDepth, SEvaluationStrategy& strategy) :
		OthelloPlayer(name, desc, maxDepth, strategy) {
	}
	~MTDfPlayer() = default;
	MTDfPlayer(const MTDfPlayer &other) = default;
	MTDfPlayer(MTDfPlayer &&other) = default;
	MTDfPlayer& operator=(const MTDfPlayer &other) = default;
	MTDfPlayer& operator=(MTDfPlayer &&other) = default;

	virtual UL getMove() {
		auto& board = m_pGameRegistry->board();
		bool blacksTurn = m_pGameRegistry->color(*this);
		short t = m_pGameRegistry->turn();
		OthelloTree tree(board, blacksTurn, t);

		short l = std::numeric_limits<short>::min();
		short u = std::numeric_limits<short>::max();

		//We use alfa-beta
		OthelloAlphaBetaTreeBuilder builder(tree, l, u, m_strategy, 3);
		MTDf mtdf(builder);

		//first guess of MTD(f) is 0 then decided by iterative deepening
		//NOTE: step 1 (last arg) seems to be faster
		short g = 0;
		IterativeDeepening id([&mtdf, &g](unsigned short depth) {
			g = mtdf.build(g, depth);
		}, m_maxDepth, 1);

		auto pCtxt = builder.validatedContext();
		return pCtxt.get() ? pCtxt->m_node->m_pos : 0L;
	}
};

} /* namespace othello */


