/*
 * JSPlayer.cpp
 *
 *  Created on: Jun 1, 2024
 *      Author: sandro
 */


#include <format>
#include <string_view>
#include <string>
#include <ranges>
#include <vector>
#include <algorithm>
#include <random>

#include "othello/player/JSPlayer.h"
#include "util/Util.h"

namespace othello {

JSPlayer::JSPlayer(std::string name, std::string desc) : Player(name, desc) {}

US JSPlayer::play() {
	auto& board = m_pGameRegistry->board();
	bool blacksTurn = m_pGameRegistry->color(*this);
	short t = m_pGameRegistry->turn();

	std::vector<std::pair<US, short>> v;
	short b = -1000;
	UL mc;
	US move = 0;

	if (board.hasMove(blacksTurn)) {
		board.board(blacksTurn).iterateOnPositions([&v, &board, &b, &mc, t, blacksTurn](UL pos) {
			short f = board.testflip(pos, blacksTurn);
			if (board.isEmpty(pos) && f) {
				US p = board.toStandard(pos);
				US d = p / 10;
				US u = p - d * 10;
				short sc = VL[(d - 1) * 8 + u - 1] - f;
				if (t > 18) {
					sc += 2 * f;
				}
				v.push_back({p, sc});
				if (sc > b) {
					b = sc;
					mc = pos;
				}
			}
			return true;
		});

		std::pair<US, short> p = Util::ramdomBestScoreMove(v);
		move = p.first;
		if (!m_silent) {
			std::cout << std::format("[J&S] Je joue en {:s} ({:d})", Util::string(move), p.second);
			std::cout << std::endl;
		}
		board.play(p.first, move);
	} else {
		if (!m_silent) std::cout << "[J&S] Je dois passer !\n";
	}
	return move;
}
} /* namespace othello */
