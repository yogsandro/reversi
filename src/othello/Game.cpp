/*
 * Game.cpp
 *
 *  Created on: Apr 14, 2026
 *      Author: sandro
 */

#include "othello/Game.h"

namespace othello {
Game::Game(GameRegistry& gameRegistry) : m_gameRegistry(gameRegistry), m_started(false) {
}

void Game::init() {
	m_gameRegistry.init();
	m_started = true;
}

void Game::end() {
	m_gameRegistry.end();
}

void Game::play(bool verbose) {
	US move = m_gameRegistry.player().play();
	m_gameRegistry.addMove(move);
	m_gameRegistry.turnOver();
	if (verbose) {
		std::cout << m_gameRegistry.board() << std::endl;
	}
}

bool Game::hasStarted() {
	return m_gameRegistry.nMoves() > 0;
}

bool Game::hasEnded() {
	auto& moves = m_gameRegistry.moves();
	auto& players = m_gameRegistry.players();
	return	!m_gameRegistry.havePlayersValidMove() ||
			(moves.back() == 0 && moves[moves.size() - 2] == 0) ||
			m_gameRegistry.countPieces(*players[0]) == 0 ||
			m_gameRegistry.countPieces(*players[1]) == 0
			;
}

} /* namespace othello */
