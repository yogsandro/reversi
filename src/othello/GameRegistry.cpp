/*
 * Game.cpp
 *
 *  Created on: May 31, 2024
 *      Author: sandro
 */


#include <iostream>
#include <algorithm>
#include <utility>
#include <format>
#include <memory>

#include "othello/GameRegistry.h"
#include "othello/Zobrist.h"
#include "Config.h"

namespace othello {

GameRegistry::GameRegistry() :
	m_turn(0),
	m_blackToPlay(true),
	m_nMoves(0) {
	m_moves.reserve(100);
}

void GameRegistry::addPlayer(Player& player) {
	if (m_players.size() >= 2) {
		throw std::runtime_error { "Le nombre maximum de joueur est de 2!" };
	}
	m_players.push_back(&player);
	player.attend(shared_from_this());
	m_map.insert({&player, true});
}

void GameRegistry::turnOver() {
	m_blackToPlay = !m_blackToPlay;
	m_nMoves++;
	m_turn = m_nMoves / 2;
}

Player& GameRegistry::player() const {
	return *m_players[!m_blackToPlay];
}

bool GameRegistry::havePlayersValidMove() {
	return m_board.hasMove(m_blackToPlay) || m_board.hasMove(!m_blackToPlay);
}

void GameRegistry::init(bool verbose) {
	m_turn = 1;
	m_blackToPlay = true;
	m_nMoves = 0;

	m_map[m_players[0]] = true;
	m_map[m_players[1]] = false;

	m_players[0]->init();
	m_players[1]->init();

	m_board.reset();

	if (verbose) {
		std::cout << "Les joueurs sont:\n";
		for (auto p : m_players) {
			std::cout << "- " << *p << (m_map.at(p) ? " (Noirs)" : " (Blancs)") << "\n";
		}
	}
/*
	Zobrist z(3450);
	z.init(m_board);
	if (verbose) std::cout << m_board << std::endl;
	bool b0, b1;
	short c0 = 0, c1 = 0;
	do {
		if (verbose) {
			std::cout << std::format("============= TOUR {:d} =============\n", m_turn);
			std::cout << std::endl;
		}
//		Config::VERBOSE = m_turn >= 27;
//		if (m_turn == 29) {
//			std::cout << std::endl;
//		}
		US move = m_players[0]->play();
		b0 = move != 0;
		addMove(move);
		if (verbose) {
			std::cout << "\n" << z.hash(true, m_board) << std::endl;
			std::cout << m_board << std::endl;
		}

		move = m_players[1]->play();
		addMove(move);
		b1 = move != 0;
		if (verbose) {
			std::cout << "\n" << z.hash(false, m_board) << std::endl;
			std::cout << m_board << std::endl;
		}
		m_turn++;
		c0 = countPieces(*m_players[0]);
		c1 = countPieces(*m_players[1]);
	} while ((b0 || b1) &&
			m_turn <= 60 &&
			c0 > 0 &&
			c1 > 0 &&
			(c0 + c1 < 64));
*/

	//std::cout << (m_board.isFull() ? "Full" : "Not full") << std::endl;
}

short GameRegistry::end(bool verbose) {
	short c0 = countPieces(*m_players[0]);
	short c1 = countPieces(*m_players[1]);

	if (verbose) {
		std::cout << m_board << std::endl;
		std::cout << std::format("La partie est terminée.\n{:s} a {:d} pion{:s} et {:s} a {:d} pion{:s}.",
				m_players[0]->name(), c0, c0 == 1 ? "" : "s", m_players[1]->name(), c1, c1 == 1 ? "" : "s");
		std::cout << std::endl;
	}

	short winner = -1;
	if (c0 > c1) {
		if (verbose) {
			std::cout << std::format("{:s} a gagné!!", m_players[0]->name());
			std::cout << std::endl;
		}
		winner = 0;
	} else if (c0 < c1) {
		if (verbose) {
			std::cout << std::format("{:s} a gagné!!", m_players[1]->name());
			std::cout << std::endl;
		}
		winner = 1;
	} else {
		if (verbose) {
			std::cout << "Ex-aequos...";
			std::cout << std::endl;
		}
	}
	m_players[0]->end();
	m_players[1]->end();

	m_players.clear();
	m_map.clear();

	return winner;

}
} /* namespace othello */
