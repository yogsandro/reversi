/*
 * MTDf3Player.h
 *
 *  Created on: Jun 13, 2025
 *      Author: sandro
 */

#pragma once

#include "othello/OthelloBoard.h"
#include "othello/player/AlphaBetaOthelloPlayer.h"
#include "builder/MTDf3.h"
#include "builder/IterativeDeepening.h"
#include "othello/AlphaBetaOthelloTree.h"


namespace othello {

/**
 * Player following the MTD(f) algorithm
 */
class MTDf3Player final : public AlphaBetaOthelloPlayer {
public:
	MTDf3Player() = delete;
	MTDf3Player(std::string name, std::string desc, unsigned short maxDepth, AlphaBetaEvaluationStrategy& strategy) :
		AlphaBetaOthelloPlayer(name, desc, maxDepth, strategy) {
	}
	~MTDf3Player() = default;
	MTDf3Player(const MTDf3Player &other) = default;
	MTDf3Player(MTDf3Player &&other) = default;
	MTDf3Player& operator=(const MTDf3Player &other) = default;
	MTDf3Player& operator=(MTDf3Player &&other) = default;

	UL getMove() {
		auto& board = m_pGameRegistry->board();
		bool blacksTurn = m_pGameRegistry->color(*this);

		AlphaBetaOthelloTree tree(board, blacksTurn);

		short l = std::numeric_limits<short>::min();
		short u = std::numeric_limits<short>::max();

		//We use alfa-beta
		AlphaBetaOthelloTreeBuilder builder(tree, l, u, m_strategy, 3);
		MTDf3 mtdf(builder);

		//first guess of MTD(f) is 0 then decided by iterative deepening
		//NOTE: step 1 (last arg) seems to be faster
		short g = 0;
		IterativeDeepening id([&mtdf, &g](unsigned short depth) {
			g = mtdf.build(g, depth);
		}, m_maxDepth, 1);

		auto pNode = builder.validated();

		return Board::convert(pNode->m_upos);
	}

};

} /* namespace othello */

