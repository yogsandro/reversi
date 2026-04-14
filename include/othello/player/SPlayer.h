/*
 * SPlayer.h
 *
 *  Created on: Jun 27, 2024
 *      Author: sandro
 */

#pragma once

#include "othello/eval/SEvaluationStrategy.h"
#include "othello/OthelloBoard.h"
#include "OthelloPlayer.h"
#include "othello/OthelloAlphaBetaTreeBuilder.h"

namespace othello {

class SPlayer final : public OthelloPlayer {
public:
	SPlayer() = delete;
	SPlayer(std::string name, std::string desc, unsigned short maxDepth, SEvaluationStrategy& strategy) :
		OthelloPlayer(name, desc, maxDepth, strategy) {
	}
	~SPlayer() = default;
	SPlayer(const SPlayer &other) = default;
	SPlayer(SPlayer &&other) = default;
	SPlayer& operator=(const SPlayer &other) = default;
	SPlayer& operator=(SPlayer &&other) = default;

	virtual UL getMove() {
		auto& board = m_pGameRegistry->board();
		bool blacksTurn = m_pGameRegistry->color(*this);
		short t = m_pGameRegistry->turn();
		OthelloTree tree(board, blacksTurn, t);
		short alpha = std::numeric_limits<short>::min();
		short beta = std::numeric_limits<short>::max();
		OthelloAlphaBetaTreeBuilder builder(tree, alpha, beta, m_strategy, m_maxDepth);
		builder.build();
		auto pCtxt = builder.validatedContext();
		return pCtxt.get() ? pCtxt->m_node->m_pos : 0L;
	}

};

} /* namespace othello */

