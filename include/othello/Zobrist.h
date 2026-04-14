/*
 * Zobrist.h
 *
 *  Created on: Jun 29, 2024
 *      Author: sandro
 */

#pragma once

#include <random>
#include <ctime>
#include <functional>

#include "OthelloBoard.h"

namespace othello {

class Zobrist;

class ZobristTable final  {
	friend Zobrist;

public:
	ZobristTable() = default;
	~ZobristTable() = default;
	ZobristTable(const ZobristTable &other) = default;
	ZobristTable(ZobristTable &&other) = default;
	ZobristTable& operator=(const ZobristTable &other) = default;
	ZobristTable& operator=(ZobristTable &&other) = default;

protected:
	long m_blackToMove;
	std::vector<std::vector<long>> m_table;

};

class Zobrist final {
public:
	Zobrist(double seed = 0) : _seed {seed}, m_rnd {
		std::uniform_int_distribution<long>(std::numeric_limits<long>::min(), std::numeric_limits<long>::max())} {
		init();
	}

	~Zobrist() = default;
	Zobrist(const Zobrist &other) = default;
	Zobrist(Zobrist &&other) = default;
	Zobrist& operator=(const Zobrist &other) = default;
	Zobrist& operator=(Zobrist &&other) = default;


	long rnd() {
		return m_rnd(m_engine);
	}

	void init(OthelloBoard& board) {
		m_table.m_blackToMove = rnd();
		m_table.m_table.push_back(std::vector<long>(64));
		m_table.m_table.push_back(std::vector<long>(64));
		board.board(true).iterateOnPositions([this, &board](UL p) {
			m_table.m_table[0][Board::toLinearPos(p)] = rnd();
			return true;
		});
		board.board(false).iterateOnPositions([this, &board](UL p) {
			m_table.m_table[1][Board::toLinearPos(p)] = rnd();
			return true;
		});
	}

	long hash(bool blackToMove, OthelloBoard& board) {
		long h = 0;
		if (blackToMove) {
			h ^= m_table.m_blackToMove;
		}

		board.board(true).iterateOnPositions([this, &board, &h](UL p) {
			if (!board.isEmpty(p)) {
				h ^= m_table.m_table[0][Board::toLinearPos(p)];
			}
			return true;
		});
		board.board(false).iterateOnPositions([this, &board, &h](UL p) {
			if (!board.isEmpty(p)) {
				h ^= m_table.m_table[1][Board::toLinearPos(p)];
			}
			return true;
		});

		return h;
	}



private:

	ZobristTable m_table;

	const double _seed;
	std::mt19937 m_engine;
	std::uniform_int_distribution<long> m_rnd;

	void init() {
		std::random_device seeder;
		const auto seed { _seed == 0 ? (seeder.entropy() ? seeder() : time(nullptr)) : _seed };
		m_engine = std::mt19937(static_cast<std::mt19937::result_type>(seed));
	}
};

} /* namespace othello */

