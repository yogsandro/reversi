/*
 * Player.h
 *
 *  Created on: May 31, 2024
 *      Author: sandro
 */

#pragma once

#include "othello/GameRegistry.h"

namespace othello {

class GameRegistry;

/**
 * An abstract player
 *
 */
class Player {
public:
	Player() = delete;
	Player(std::string name, std::string desc = "");
	virtual ~Player() = default;
	Player(const Player &other) = default;
	Player(Player &&other) = default;
	Player& operator=(const Player &other) = default;
	Player& operator=(Player &&other) = default;

	virtual void init() {
	}

	virtual void end() {
	}

	virtual US play() = 0;

	const std::string& name() const { return m_name; };

	//bool operator<(Player& src) const;
	//std::strong_ordering operator<=>(const Player&) const;

	friend std::ostream& operator<<(std::ostream &f, const Player& player);

	void setSilent(bool silent) {
		m_silent = silent;
	}

	void attend(std::shared_ptr<GameRegistry> game) {
		m_pGameRegistry = game;
	}

protected:
	std::string m_name;
	std::string m_desc;

	bool m_silent;

	std::shared_ptr<GameRegistry> m_pGameRegistry;
};

} /* namespace othello */

