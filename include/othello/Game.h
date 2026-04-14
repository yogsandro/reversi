/*
 * Game.h
 *
 *  Created on: Apr 14, 2026
 *      Author: sandro
 */

#pragma once

#include "GameRegistry.h"

namespace othello {
class Game {
public:
	Game(GameRegistry&);
	virtual ~Game() = default;

	void init();
	void end();
	void play(bool verbose = false);
	bool hasEnded();
private:
	GameRegistry& m_gameRegistry;
};

} /* namespace othello */
