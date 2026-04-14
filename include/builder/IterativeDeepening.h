/*
 * iterativeDeepening.h
 *
 *  Created on: Jun 30, 2024
 *      Author: sandro
 */

#pragma once
#include <iostream>
#include <functional>
#include <chrono>

#include "../Config.h"

namespace othello {

class IterativeDeepening final {
public:
	IterativeDeepening() = delete;
	IterativeDeepening(std::function<void(unsigned short)> iterativeFunction, unsigned short maxTreeDepth,
			unsigned short step = 1) :
		m_iterativeFunction (iterativeFunction),
		m_maxTreeSearchDepth (maxTreeDepth),
		m_step (step) {
		build();
	};

	~IterativeDeepening() = default;
	IterativeDeepening(const IterativeDeepening &other) = default;
	IterativeDeepening(IterativeDeepening &&other) = default;
	IterativeDeepening& operator=(const IterativeDeepening &other) = default;
	IterativeDeepening& operator=(IterativeDeepening &&other) = default;

	bool verbose = Config::VERBOSE;


	void build() {
		unsigned short currentDepth = 1;

		auto start = std::chrono::steady_clock::now();
		auto s = start;

		while(currentDepth <= m_maxTreeSearchDepth) {


			//m_builder.setTree(copy);
			if (verbose) std::cout << "\n============================Start D=" << currentDepth  << "============================n" << std::flush;

			//call the iterative function of this iterativeDeepening
			m_iterativeFunction(currentDepth);

			auto end = std::chrono::steady_clock::now();
			auto d = std::chrono::duration_cast<std::chrono::milliseconds> (end - s).count();
			s = end;
			auto t = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();

			if (verbose) {
				std::cout << "\n============================End D=" << currentDepth  << "============================\n" << std::flush;
				std::cout << "\nT(" << currentDepth << ")=" << d << " ms, Total=" << t << " ms." << std::flush;
			}

			currentDepth += m_step;
			if (currentDepth > m_maxTreeSearchDepth && currentDepth - m_step < m_maxTreeSearchDepth) {
				currentDepth = m_maxTreeSearchDepth;
			}
		}

	}

protected:
	std::function<void(unsigned short)> m_iterativeFunction;
	unsigned short m_maxTreeSearchDepth;
	unsigned short m_step;

private:

};

} /* namespace othello */

