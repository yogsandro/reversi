/*
 * JSPlayer.h
 *
 *  Created on: Jun 1, 2024
 *      Author: sandro
 */

#pragma once

#include "Player.h"

namespace othello {

/**
 * Jeux&Stratégie player. Its algorithm is the original from J&S n35
 */
class JSPlayer final : public Player {
public:
	JSPlayer() = delete;
	JSPlayer(std::string name, std::string desc);
	~JSPlayer() = default;
	JSPlayer(const JSPlayer &other) = default;
	JSPlayer(JSPlayer &&other) = default;
	JSPlayer& operator=(const JSPlayer &other) = default;
	JSPlayer& operator=(JSPlayer &&other) = default;

	US play() override;

private:
	static constexpr short VL[] = {
			40,  3, 15, 10, 10, 15,  3, 40,
			3,   0,  9, 12, 12,  9,  0,  3,
			15,  9, 11, 15, 15, 11,  9, 15,
			10, 12, 15, 18, 18, 15, 12, 10,
			10, 12, 15, 18, 18, 15, 12, 10,
			15,  9, 11, 15, 15, 11,  9, 15,
			3,   0,  9, 12, 12,  9,  0,  3,
			40,  3, 15, 10, 10, 15,  3, 40,
	};

};

} /* namespace othello */

