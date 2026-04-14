/*
 * Game.h
 *
 *  Created on: May 31, 2024
 *      Author: sandro
 */

#pragma once

#include <map>
#include <vector>
#include "OthelloBoard.h"
#include "OthelloTree.h"
#include "player/Player.h"

namespace othello {

class Player;
class GameRegistry final : public std::enable_shared_from_this<GameRegistry> {
public:
	GameRegistry();
	~GameRegistry() = default;
	GameRegistry(const GameRegistry &other) = default;
	GameRegistry(GameRegistry &&other) = default;
	GameRegistry& operator=(const GameRegistry &other) = default;
	GameRegistry& operator=(GameRegistry &&other) = default;

	void addPlayer(Player& player);
	void init(bool verbose = true);
	short end(bool verbose = true);

	void turnOver();

	Player& player() const;

	OthelloBoard& board() {
		return m_board;
	}

	bool color(Player& player) {
		return m_map.at(&player);
	}

	short countPieces(Player& player) {
		return m_board.count(color(player));
	}

	short turn() {
		return m_turn;
	}

	const std::vector<UL>& moves() {
		return m_moves;
	}

	void addMove(UL pos) {
		std::cout << "\nS=" << m_moves.size() << std::flush;
		m_moves.push_back(pos);
	}

	UL latestMove() {
		return m_moves.back();
	}

	const std::vector<Player*>& players() const {
		return m_players;
	}

	bool havePlayersValidMove();


private:
	OthelloBoard m_board;
	std::vector<Player*> m_players;
	std::map<Player*, bool> m_map;
	std::vector<UL> m_moves;
	short m_turn;
	bool m_blacksTurn;
	short m_play;
};

} /* namespace othello */

