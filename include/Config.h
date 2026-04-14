/*
 * Config.h
 *
 *  Created on: Jul 3, 2024
 *      Author: sandro
 */

#pragma once

namespace othello {

class Config final {
public:
	Config() = delete;
	~Config() = default;
	Config(const Config &other) = default;
	Config(Config &&other) = default;
	Config& operator=(const Config &other) = default;
	Config& operator=(Config &&other) = default;

	static bool VERBOSE;
};

} /* namespace othello */

