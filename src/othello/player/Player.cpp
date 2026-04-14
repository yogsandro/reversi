/*
 * Player.cpp
 *
 *  Created on: May 31, 2024
 *      Author: sandro
 */

#include "othello/player/Player.h"

namespace othello {

Player::Player(std::string name, std::string desc) : m_name(name), m_desc(desc), m_silent(false) {
}

std::ostream& operator<<(std::ostream &f, const Player& player) {
	f << player.m_name;
	if (player.m_desc.length() > 0) {
		f << " (" << player.m_desc << ")";
	}
	return f;
}

} /* namespace othello */
