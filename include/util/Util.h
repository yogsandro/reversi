/*
 * Util.h
 *
 *  Created on: Jun 10, 2024
 *      Author: sandro
 */

#ifndef UTIL_H_
#define UTIL_H_

#include <vector>
#include <iostream>
#include <ranges>
#include <algorithm>
#include <random>


namespace othello {

class Util final {
public:
	Util() = default;
	~Util() = default;
	Util(const Util &other) = default;
	Util(Util &&other) = default;
	Util& operator=(const Util &other) = default;
	Util& operator=(Util &&other) = default;

	static std::string itoa(short a) {
		return (a == std::numeric_limits<short>::min() ? "-\u221E" :
				a == std::numeric_limits<short>::max() ? "+\u221E" : std::to_string(a));
	}

	static std::string string(unsigned short p) {
		unsigned short d = p / 10 - 1, u = p - 10 * (d + 1);
		return std::string(1, 'A' + d) + std::to_string(u);
	}

	/**
	 * Return a random move (+ its score in a pair) chosen from the best possible moves
	 * @param v
	 * @return
	 */
	static std::pair<unsigned short, short> ramdomBestScoreMove(std::vector<std::pair<unsigned short, short>> &v) {
		//sort pairs based on score
		std::sort(v.begin(), v.end(),
				[](std::pair<int, double> pi, std::pair<unsigned short, short> pj) {
			return pi.second > pj.second;
		});

		//print(v);
		//std::cout << "n= " << n(v) << '\n';

		//take the best score. We now have at least one pair or more pairs with same scores
		auto r = v | std::views::take(n(v)) | std::views::common;

		//Build a vector from the view...
		std::vector<std::pair<unsigned short, short>> filtered = std::vector(r.begin(), r.end());

		//...and shuffle the vector
		std::random_device rd;
		std::mt19937 g(rd());
		std::shuffle(filtered.begin(), filtered.end(), g);

		//just return the first pair
		return filtered[0];
	}

	/**
	 * Lambda which compute the number of identical score (pair.second). the vector must be sorted
	 */
	constexpr static auto n = [](const std::vector<std::pair<unsigned short, short>> &v) {
		int k { 1 };
		for (auto it = v.begin() + 1; it != v.end() && (*it).second == (*(it - 1)).second; ++it) {
			++k;
		}
		return k;
	};

	/**
	 * Lambda for pair printing
	 */
	constexpr static auto print = [](const std::vector<std::pair<unsigned short, short>> &v,
			std::string_view const rem = "") {
		for (auto a : v) {
			std::cout << "(" << a.first << ", " << a.second << "), ";
		}
		std::cout << ": " << rem << '\n';
	};

};

} /* namespace othello */

#endif /* UTIL_H_ */
