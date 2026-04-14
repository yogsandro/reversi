/*
 * HumanPlayer.cpp
 *
 *  Created on: May 31, 2024
 *      Author: sandro
 */


#include <iostream>
#include <format>

#include "othello/player/HumanPlayer.h"

namespace othello {

HumanPlayer::HumanPlayer(std::string name, std::string desc) : Player(name, desc) {}

US HumanPlayer::play() {
	auto& board = m_pGameRegistry->board();
	bool blacksTurn = m_pGameRegistry->color(*this);

	US move = 0;
	if (board.hasMove(blacksTurn)) {
		char d;
		US u;
		do {
			std::cout << "Que jouez-vous ? ";
			std::cin >> d >> u;
			move = (int(d) - 96) * 10 + u;
		} while(!board.canPlay(move, blacksTurn));
		board.play(move, blacksTurn);
	} else {
		std::cout << "Vous devez passer !\n";
	}
	return move;
}
} /* namespace othello */
