/*
 * RandomEngineHelper.h
 *
 *  Created on: Jan 2, 2024
 *      Author: sandro
 */

#pragma once
#include <iostream>
#include <random>
#include <ctime>

namespace rnd {

class RandomEngineHelper final {
public:
//	RandomEngineHelper() : _seed {0} {
//		init();
//	}

	static RandomEngineHelper& instance() {
		static RandomEngineHelper single;
		return single;
	}

	virtual ~RandomEngineHelper() {
		// TODO Auto-generated destructor stub
	}

	std::mt19937 operator()() {
			return m_engine;
	}

	std::mt19937 gen() const {
		return m_engine;
	}

	double uniform() {
		return m_uniform_states(m_engine);
	}

	double uniform_real(double a, double b) {
		return a + (b - a) * uniform();
	}

	int uniform_int(long unsigned int s) {
		return static_cast<int>(uniform() * s);
	}

	void set_seed(double seed) {
		_seed = seed;
		init();
	}

	double seed() {
		return _seed;
	}

private:
	double _seed;
	std::mt19937 m_engine;
	std::uniform_real_distribution<> m_uniform_states;

	RandomEngineHelper(double seed = 0) : _seed {seed}, m_uniform_states {std::uniform_real_distribution<>(0, 1)} {
		init();
	}

	void init() {
		//std::cout << "initializing random engine with seed " << _seed << std::flush;
		std::random_device seeder;
		const auto seed { _seed == 0 ? (seeder.entropy() ? seeder() : time(nullptr)) : _seed };
		m_engine = std::mt19937(static_cast<std::mt19937::result_type>(seed));
	}

	RandomEngineHelper(const RandomEngineHelper& other) = delete;
	RandomEngineHelper& operator=(const RandomEngineHelper& other) = delete;
};

} /* namespace ml */

