/*
 * IDPlayer.h
 *
 *  Created on: Jul 0, 2024
 *      Author: sandro
 */

#pragma once

#include "othello/eval/SEvaluationStrategy.h"
#include "othello/OthelloBoard.h"
#include "Player.h"

#include "Config.h"


namespace othello {

class OthelloPlayer : public Player {
public:
	OthelloPlayer() = delete;
	OthelloPlayer(std::string name, std::string desc, unsigned short maxDepth, SEvaluationStrategy& strategy) :
		Player(name, desc),
		m_strategy(strategy),
		m_maxDepth(maxDepth) {
	}
	~OthelloPlayer() = default;
	OthelloPlayer(const OthelloPlayer &other) = default;
	OthelloPlayer(OthelloPlayer &&other) = default;
	OthelloPlayer& operator=(const OthelloPlayer &other) = default;
	OthelloPlayer& operator=(OthelloPlayer &&other) = default;

	US play() {
		US move = 0;
		auto& board = m_pGameRegistry->board();
		bool blacksTurn = m_pGameRegistry->color(*this);
		auto pos = getMove();
//##			game.addMove(pos);

		if (pos == 0) {
			if (!m_silent) std::cout << "[" << m_name << "] Je dois passer !\n";
		} else {
			move = Board::convert(pos);

			if (!m_silent) {
				std::cout << std::format("\n[{:s}] Je joue en {:s}", m_name, Util::string(move));//result.second);
				std::cout << std::endl;
			}
			board.play(move, blacksTurn);
		}
		return move;
	}

	virtual UL getMove() = 0;

protected:
	SEvaluationStrategy& m_strategy;
	unsigned short m_maxDepth;


};

} /* namespace othello */

