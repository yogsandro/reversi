/*
 * AlphaBetaOthelloPlayer.h
 *
 *  Created on: Jun 12, 2025
 *      Author: sandro
 */

#pragma once
#include "othello/OthelloBoard.h"
#include "othello/AlphaBetaOthelloTree.h"
#include "Player.h"


namespace othello {

class AlphaBetaOthelloPlayer : public Player {
public:
	AlphaBetaOthelloPlayer() = delete;
	AlphaBetaOthelloPlayer(std::string name, std::string desc, unsigned short maxDepth, AlphaBetaEvaluationStrategy& strategy) :
		Player(name, desc),
		m_strategy(strategy),
		m_maxDepth(maxDepth) {
	}
	~AlphaBetaOthelloPlayer() = default;
	AlphaBetaOthelloPlayer(const AlphaBetaOthelloPlayer &other) = default;
	AlphaBetaOthelloPlayer(AlphaBetaOthelloPlayer &&other) = default;
	AlphaBetaOthelloPlayer& operator=(const AlphaBetaOthelloPlayer &other) = default;
	AlphaBetaOthelloPlayer& operator=(AlphaBetaOthelloPlayer &&other) = default;

	US play() {
		auto& board = m_pGameRegistry->board();
		bool blacksTurn = m_pGameRegistry->color(*this);
		US move = 0;
		auto pos = getMove();
		if (pos == 0) {
			if (!m_silent) std::cout << "[" << m_name << "] Je dois passer !\n";
		} else {
			move = Board::convert(pos);
			std::cout << move << std::flush;
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
	AlphaBetaEvaluationStrategy& m_strategy;
	unsigned short m_maxDepth;


};

} /* namespace othello */

