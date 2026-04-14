/*
 * HumanPlayer.h
 *
 *  Created on: May 31, 2024
 *      Author: sandro
 */

#pragma once

#include "Player.h"

namespace othello {

/**
 * A Human player
 */
class HumanPlayer final : public Player {
public:
	HumanPlayer() = delete;
	HumanPlayer(std::string name, std::string desc = "");
	~HumanPlayer() = default;
	HumanPlayer(const HumanPlayer &other) = default;
	HumanPlayer(HumanPlayer &&other) = default;
	HumanPlayer& operator=(const HumanPlayer &other) = default;
	HumanPlayer& operator=(HumanPlayer &&other) = default;

	US play() override;
};

} /* namespace othello */

