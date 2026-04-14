/*
 * GPlayer.h
 *
 *  Created on: Jun 2, 2024
 *      Author: sandro
 */

#pragma once


#include "othello/eval/GEvaluationStrategy.h"
#include "othello/OthelloBoard.h"
#include "Player.h"


namespace othello {


/**
 * Player based on Giuseppe's algorithm
 */
class GPlayer final : public Player {
public:
	GPlayer() = delete;
	GPlayer(std::string name, std::string desc, GEvaluationStrategy& strategy) :
		Player(name, desc),
		m_treeStrategy(strategy) {
	}
	~GPlayer() = default;
	GPlayer(const GPlayer &other) = default;
	GPlayer(GPlayer &&other) = default;
	GPlayer& operator=(const GPlayer &other) = default;
	GPlayer& operator=(GPlayer &&other) = default;

	US play() override;

private:
	GEvaluationStrategy& m_treeStrategy;

	void build(OthelloBoard& board);
	void eval(OthelloBoard& board, bool piece, UL cs, short ret);

};

} /* namespace othello */

