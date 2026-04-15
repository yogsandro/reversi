/*
 * OthelloBoard.h
 *
 *  Created on: Jun 5, 2024
 *      Author: sandro
 */

#pragma once

#include <bitset>
#include <vector>
#include <algorithm>
#include <vector>
#include <ostream>
#include <iostream>
#include <execution>

namespace othello {

using UL = unsigned long;
using US = unsigned short;

/**
 * A Board specific to a color (black or white). It uses two unsigned long coding
 *
 * High part of the board
 * XXXX
 * XXXXXXXXXX
 * XOOOOOOOOX
 * XOOOOOOOOX
 * XOOOOOOOOX
 * XOOOOOOOOX
 * XXXXXXXXXX
 *
 * Low part of the board
 * XXXX
 * XXXXXXXXXX
 * XOOOOOOOOX
 * XOOOOOOOOX
 * XOOOOOOOOX
 * XOOOOOOOOX
 * XXXXXXXXXX
 *
 * The Xs mark the board border and the Os the board itself (we have a 2x4x8=64 grid)
 *
 * For a given unsigned long value position (aka the internal value position), the highest
 * bit of the last 4 bits (1L << 63) is used to indicate to which board the value belongs to.
 *
 * For example:
 *
 * 1000 0000000000 0100000000 0000000000 0000000000 0000000000 0000000000
 * 1000 0000000000 0010000000 0000000000 0000000000 0000000000 0000000000
 * 1000 0000000000 0001000000 0000000000 0000000000 0000000000 0000000000
 * 1000 0000000000 0000100000 0000000000 0000000000 0000000000 0000000000
 * 0000 0000000000 0000000000 0000000000 0000000000 0000000100 0000000000
 * |  | |        | |        | |        | |        | |        | |        |
 * 63   59      50 49      40 39      30 29      20 19      10 9        0
 *
 * are the A1,B1,C1,D1 and F8 positions. A1,B1,C1,D1 are in the high part (with the highest
 * bit set to 1) and F8 is in the low part (with the highest bit set to 0)
 *
 */
struct Board {

	//clockwise shift to compute neighbours of a position
	static constexpr short SHIFT[8] = {-11,-10,-9,1,11,10,9,-1};
	static constexpr UL H =         0b1111111111111111111111111111111100000000000000000000000000000000;
	static constexpr UL L =         0b0000000000000000000000000000000011111111111111111111111111111111;
	static constexpr UL HIGH_BITS = 0b1111000000000000000000000000000000000000000000000000000000000000;
	static constexpr UL INTERIOR  = 0b0000000000000001111111100111111110011111111001111111100000000000;
	static constexpr UL FULL =      0b0000111111111111111111111111111111111111111111111111111111111111;
	static constexpr UL BORDER = ~INTERIOR;
	static constexpr UL HIGH_BORDER = 0b111111110;
	static constexpr UL LOW_BORDER = HIGH_BORDER << 50;
	static constexpr UL HIGH_TYPE = 1L << 63;
	static constexpr UL NOT_HIGH_TYPE = ~HIGH_TYPE;

	static constexpr UL CORNER_TOP_LEFT = (1L << 48) | HIGH_TYPE;
	static constexpr UL CORNER_TOP_RIGHT = (1L << 41) | HIGH_TYPE;
	static constexpr UL CORNER_BOTTOM_RIGHT = 1L << 11;
	static constexpr UL CORNER_BOTTOM_LEFT = 1L << 18;


	static constexpr UL H0 = 1L << 15;
	static constexpr UL L0 = 1L << 45;

	static const std::vector<UL> POS_;
	static const UL NEIGHBORS_[548];

	//array of neighborhood: [{first = position, 2nd=low neighborhood, Third = high neighborhood}]
	static constexpr UL NEIGHBORHOODS[192] = {
		0b0000000000000000000000000000000000000000000000000000100000000000, 0b0000000000000000000000000000000000000000011000000001000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000000000000001000000000000, 0b0000000000000000000000000000000000000000111000000010100000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000000000000010000000000000, 0b0000000000000000000000000000000000000001110000000101000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000000000000100000000000000, 0b0000000000000000000000000000000000000011100000001010000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000000000001000000000000000, 0b0000000000000000000000000000000000000111000000010100000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000000000010000000000000000, 0b0000000000000000000000000000000000001110000000101000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000000000100000000000000000, 0b0000000000000000000000000000000000011100000001010000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000000001000000000000000000, 0b0000000000000000000000000000000000011000000000100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000001000000000000000000000, 0b0000000000000000000000000000000110000000010000000001100000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000010000000000000000000000, 0b0000000000000000000000000000001110000000101000000011100000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000000100000000000000000000000, 0b0000000000000000000000000000011100000001010000000111000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000001000000000000000000000000, 0b0000000000000000000000000000111000000010100000001110000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000010000000000000000000000000, 0b0000000000000000000000000001110000000101000000011100000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000000100000000000000000000000000, 0b0000000000000000000000000011100000001010000000111000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000001000000000000000000000000000, 0b0000000000000000000000000111000000010100000001110000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000000010000000000000000000000000000, 0b0000000000000000000000000110000000001000000001100000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000010000000000000000000000000000000, 0b0000000000000000000001100000000100000000011000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000000100000000000000000000000000000000, 0b0000000000000000000011100000001010000000111000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000001000000000000000000000000000000000, 0b0000000000000000000111000000010100000001110000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000010000000000000000000000000000000000, 0b0000000000000000001110000000101000000011100000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000000100000000000000000000000000000000000, 0b0000000000000000011100000001010000000111000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000001000000000000000000000000000000000000, 0b0000000000000000111000000010100000001110000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000010000000000000000000000000000000000000, 0b0000000000000001110000000101000000011100000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000000100000000000000000000000000000000000000, 0b0000000000000001100000000010000000011000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000,
		0b0000000000000000000000100000000000000000000000000000000000000000, 0b0000000000000000000001000000000110000000000000000000000000000000, 0b1000000000000000000000000000000000000000000000000001100000000000,
		0b0000000000000000000001000000000000000000000000000000000000000000, 0b0000000000000000000010100000001110000000000000000000000000000000, 0b1000000000000000000000000000000000000000000000000011100000000000,
		0b0000000000000000000010000000000000000000000000000000000000000000, 0b0000000000000000000101000000011100000000000000000000000000000000, 0b1000000000000000000000000000000000000000000000000111000000000000,
		0b0000000000000000000100000000000000000000000000000000000000000000, 0b0000000000000000001010000000111000000000000000000000000000000000, 0b1000000000000000000000000000000000000000000000001110000000000000,
		0b0000000000000000001000000000000000000000000000000000000000000000, 0b0000000000000000010100000001110000000000000000000000000000000000, 0b1000000000000000000000000000000000000000000000011100000000000000,
		0b0000000000000000010000000000000000000000000000000000000000000000, 0b0000000000000000101000000011100000000000000000000000000000000000, 0b1000000000000000000000000000000000000000000000111000000000000000,
		0b0000000000000000100000000000000000000000000000000000000000000000, 0b0000000000000001010000000111000000000000000000000000000000000000, 0b1000000000000000000000000000000000000000000001110000000000000000,
		0b0000000000000001000000000000000000000000000000000000000000000000, 0b0000000000000000100000000110000000000000000000000000000000000000, 0b1000000000000000000000000000000000000000000001100000000000000000,
		0b1000000000000000000000000000000000000000000000000000100000000000, 0b0000000000000000000001100000000000000000000000000000000000000000, 0b1000000000000000000000000000000000000000011000000001000000000000,
		0b1000000000000000000000000000000000000000000000000001000000000000, 0b0000000000000000000011100000000000000000000000000000000000000000, 0b1000000000000000000000000000000000000000111000000010100000000000,
		0b1000000000000000000000000000000000000000000000000010000000000000, 0b0000000000000000000111000000000000000000000000000000000000000000, 0b1000000000000000000000000000000000000001110000000101000000000000,
		0b1000000000000000000000000000000000000000000000000100000000000000, 0b0000000000000000001110000000000000000000000000000000000000000000, 0b1000000000000000000000000000000000000011100000001010000000000000,
		0b1000000000000000000000000000000000000000000000001000000000000000, 0b0000000000000000011100000000000000000000000000000000000000000000, 0b1000000000000000000000000000000000000111000000010100000000000000,
		0b1000000000000000000000000000000000000000000000010000000000000000, 0b0000000000000000111000000000000000000000000000000000000000000000, 0b1000000000000000000000000000000000001110000000101000000000000000,
		0b1000000000000000000000000000000000000000000000100000000000000000, 0b0000000000000001110000000000000000000000000000000000000000000000, 0b1000000000000000000000000000000000011100000001010000000000000000,
		0b1000000000000000000000000000000000000000000001000000000000000000, 0b0000000000000001100000000000000000000000000000000000000000000000, 0b1000000000000000000000000000000000011000000000100000000000000000,
		0b1000000000000000000000000000000000000000001000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000000000110000000010000000001100000000000,
		0b1000000000000000000000000000000000000000010000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000000001110000000101000000011100000000000,
		0b1000000000000000000000000000000000000000100000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000000011100000001010000000111000000000000,
		0b1000000000000000000000000000000000000001000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000000111000000010100000001110000000000000,
		0b1000000000000000000000000000000000000010000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000001110000000101000000011100000000000000,
		0b1000000000000000000000000000000000000100000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000011100000001010000000111000000000000000,
		0b1000000000000000000000000000000000001000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000111000000010100000001110000000000000000,
		0b1000000000000000000000000000000000010000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000000000110000000001000000001100000000000000000,
		0b1000000000000000000000000000000010000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000001100000000100000000011000000000000000000000,
		0b1000000000000000000000000000000100000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000011100000001010000000111000000000000000000000,
		0b1000000000000000000000000000001000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000111000000010100000001110000000000000000000000,
		0b1000000000000000000000000000010000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000001110000000101000000011100000000000000000000000,
		0b1000000000000000000000000000100000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000011100000001010000000111000000000000000000000000,
		0b1000000000000000000000000001000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000111000000010100000001110000000000000000000000000,
		0b1000000000000000000000000010000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000001110000000101000000011100000000000000000000000000,
		0b1000000000000000000000000100000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000001100000000010000000011000000000000000000000000000,
		0b1000000000000000000000100000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000001000000000110000000000000000000000000000000,
		0b1000000000000000000001000000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000010100000001110000000000000000000000000000000,
		0b1000000000000000000010000000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000000101000000011100000000000000000000000000000000,
		0b1000000000000000000100000000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000001010000000111000000000000000000000000000000000,
		0b1000000000000000001000000000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000010100000001110000000000000000000000000000000000,
		0b1000000000000000010000000000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000101000000011100000000000000000000000000000000000,
		0b1000000000000000100000000000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000001010000000111000000000000000000000000000000000000,
		0b1000000000000001000000000000000000000000000000000000000000000000, 0b0000000000000000000000000000000000000000000000000000000000000000, 0b1000000000000000100000000110000000000000000000000000000000000000,
	};

	unsigned long low;
	unsigned long high;

	Board(UL l = 0, UL h = 0) : low (l), high(h) {};
	~Board() = default;
	Board(const Board &other) = default;
	Board(Board &&other) = default;
	Board& operator=(const Board &other) = default;
	Board& operator=(Board &&other) = default;

	void reset(UL l, UL h) {
		low = l;
		high = h;
	}

	/**
	 * Iterate on valid board positions
	 * @tparam Function
	 * @param function
	 */
	template<class Function>
	constexpr void iterateOnPositions(Function function) const {
		US k {11};
		UL p = 1L << k;
		do {
			//UL p = 1L << i;
			if (p & INTERIOR) {
				if (!function(p) || !function(Board::HIGH_TYPE | p)) {
					break;
				}
			}
			p <<= 1;
		} while (++k < 49);
	}

	template<class Function>
	constexpr void iterateOnPositionsPar(Function function) {
		std::for_each(std::execution::par_unseq, POS_.begin(), POS_.end(),
			[&function](unsigned long p) {
				return function(p);
			}
		);
	}


	/**
	 * Iterate on valid board positions
	 * @tparam Function
	 * @param function
	 */
	template<class Function>
	constexpr void iterateOnPositionsCornersFirst(Function function) const {
		US k {11};
		UL p = 1L << k;

		if (function(p)) {
			p <<= 7;
			if (function(p)) {
				p <<= 23;
				if (function(p | HIGH_TYPE)) {
					p <<= 7;
					if (function(p | HIGH_TYPE)) {
						p = 1L << k;
						do {
							if (p & INTERIOR) {
								if ((p != CORNER_BOTTOM_RIGHT) &&
									(p != CORNER_BOTTOM_LEFT)) {
									if (!function(p)) {
										break;
									}
								}
								if (((p | HIGH_TYPE ) != CORNER_TOP_RIGHT) &&
									((p | HIGH_TYPE ) != CORNER_TOP_LEFT)) {
									if (!function(HIGH_TYPE | p)) {
										break;
									}
								}
							}
							p <<= 1;
						} while (++k < 49);
					}
				}
			}
		}
	}

	template<class Function>
	constexpr void simpleIterateOnEmptyPositionsCornersFirst(const Board& other, Function function) const {
		const UL l = other.low | low;
		const UL h = other.high | high;
		US k {11};
		UL p = 1L << k;

		if (!(p & l)) {
			function(p);
		}
		p <<= 7;
		if (!(p & l)) {
			function(p);
		}
		p <<= 23;
		if (!(p & h)) {
			function(p | HIGH_TYPE);
		}
		p <<= 7;
		if (!(p & h)) {
			function(p | HIGH_TYPE);
		}
		p = 1L << k;
		do {
			if (p & INTERIOR) {
				if ((p != CORNER_BOTTOM_RIGHT) &&
					(p != CORNER_BOTTOM_LEFT)) {
					if (!(p & l)) function(p);
				}
				if (((p | HIGH_TYPE ) != CORNER_TOP_RIGHT) &&
					((p | HIGH_TYPE ) != CORNER_TOP_LEFT)) {
					if (!(p & h)) function(HIGH_TYPE | p);
				}
			}
			p <<= 1;
		} while (++k < 49);
	}

	template<class Function>
	constexpr void simpleIterateOnEmptyPositions(const Board& other, Function function) const {
		const UL l = other.low | low;
		const UL h = other.high | high;
		US k {11};
		UL p = 1L << k;
		do {
			if (p & INTERIOR) {
				if (!(p & l)) function(p);
				if (!(p & h)) function(HIGH_TYPE | p);
			}
			p <<= 1;
		} while (++k < 49);
	}

	template<class Function>
	constexpr bool anyOfEmptyPositions(const Board& other, Function function) const {
		const UL l = other.low | low;
		const UL h = other.high | high;
		US k {11};
		UL p = 1L << k;
		do {
			if (p & INTERIOR) {
				if (!(p & l)) {
					if (function(p)) {
						return true;;
					}
				}
				if (!(p & h)) {
					if (function(HIGH_TYPE | p)) {
						return true;
					}
				}
			}
			p <<= 1;
		} while (++k < 49);

		return false;
	}

	/**
	 * Iterate on valid board positions
	 * @tparam Function
	 * @param function
	 */
	template<class Function>
	constexpr bool doWhileOnPositions(Function function) const {
		US k {11};
		UL p = 1L << k;
		do {
			//UL p = 1L << i;
			if (p & INTERIOR) {
				if (function(p) || function(Board::HIGH_TYPE | p)) {
					return true;
				}
			}
			p <<= 1;
		} while (++k < 49);
		return false;
	}

	/**
	 * Iterate on valid board in ordered positions : A1,A2,...B1,B2,...G8,H8
	 * @tparam Function
	 * @param function
	 */
	template<class Function>
	constexpr void iterateOnOrderedPositions(Function function) {
		//high
		for (US i {48}; i >= 11; --i) {
			UL p = 1L << i;
			if (p & INTERIOR) {
				function(Board::HIGH_TYPE | p);
			}
		}
		//low
		for (US i {48}; i >= 11; --i) {
			UL p = 1L << i;
			if (p & INTERIOR) {
				function(p);
			}
		}
	}

	/**
	 * Neighbors template loop
	 * @tparam Function
	 * @param pos
	 * @param f
	 */
	template<class Function>
	constexpr void iterateOnNeighbors(const UL pos, Function f) {
		for (const auto& s : SHIFT) {
			UL p = neighbor(pos, s);
			if (p & INTERIOR) {
				if (!f(p, s)) {
					break;
				}
			}
		}
	}

	//optimized version of iterateOnNeighbors
	template<class Function>
	constexpr void iterateOnNeighbors_(const UL pos, Function f) {
		UL p = pos;
		bool highType = p & HIGH_TYPE;

		//SHIFT[8] = {-11,-10,-9,1,11,10,9,-1};
		if (highType) {
			//remove high type from p else it will be shifted
			p &= NOT_HIGH_TYPE;

			UL q = p << 11;
			if (q & INTERIOR) {
				f(q | HIGH_TYPE, -11);
			}

			q = p << 10;
			if (q & INTERIOR) {
				f(q | HIGH_TYPE, -10);
			}

			q = p << 9;
			if (q & INTERIOR) {
				f(q | HIGH_TYPE, -9);
			}

			q = p >> 1;
			if (q & INTERIOR) {
				f(q | HIGH_TYPE, 1);
			}

			highType = true;
			q = (p >> 11) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (highType) {
				q |= HIGH_TYPE;
			}
			if (q & INTERIOR) {
				f(q, 11);
			}

			highType = true;
			q = (p >> 10) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (highType) {
				q |= HIGH_TYPE;
			}
			if (q & INTERIOR) {
				f(q, 10);
			}

			highType = true;
			q = (p >> 9) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (highType) {
				q |= HIGH_TYPE;
			}
			if (q & INTERIOR) {
				f(q, 9);
			}

			q = p << 1;
			if (q & INTERIOR) {
				f(q | HIGH_TYPE, -1);
			}
		} else {
			highType = false;
			UL q = p << 11;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (highType) {
				q |= HIGH_TYPE;
			}
			if (q & INTERIOR) {
				f(q, -11);
			}


			highType = false;
			q = p << 10;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (highType) {
				q |= HIGH_TYPE;
			}
			if (q & INTERIOR) {
				f(q, -10);
			}

			highType = false;
			q = p << 9;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (highType) {
				q |= HIGH_TYPE;
			}
			if (q & INTERIOR) {
				f(q, -9);
			}

			q = p >> 1;
			if (q & INTERIOR) {
				f(q, 1);
			}

			q = p >> 11;
			if (q & INTERIOR) {
				f(q, 11);
			}

			q = p >> 10;
			if (q & INTERIOR) {
				f(q, 10);
			}

			q = p >> 9;
			if (q & INTERIOR) {
				f(q, 9);
			}

			q = p << 1;
			if (q & INTERIOR) {
				f(q, -1);
			}
		}
	}

	///optimized version of iterateOnNeighbors plus call method if neighbor of pos is
	///occupied by other board
	template<class Function>
	constexpr void simpleIterateOnOccupiedNeighbors_(const UL pos, const Board& b, Function f) {
		UL p = pos;
		bool highType = p & HIGH_TYPE;

		//SHIFT[8] = {-11,-10,-9,1,11,10,9,-1};
		if (highType) {
			//remove high type from p else it will be shifted
			p &= NOT_HIGH_TYPE;

			UL q = p << 11;
			if ((q & INTERIOR) && (q & b.high)) {
				f(q | HIGH_TYPE, -11);
			}

			q = p << 10;
			if ((q & INTERIOR) && (q & b.high)) {
				f(q | HIGH_TYPE, -10);
			}

			q = p << 9;
			if ((q & INTERIOR) && (q & b.high)) {
				f(q | HIGH_TYPE, -9);
			}

			q = p >> 1;
			if ((q & INTERIOR) && (q & b.high)) {
				f(q | HIGH_TYPE, 1);
			}

			highType = true;
			q = (p >> 11) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						f(q | HIGH_TYPE, 11);
					}
				} else if (q & b.low) {
					f(q, 11);
				}
			}

			highType = true;
			q = (p >> 10) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						f(q | HIGH_TYPE, 10);
					}
				} else if (q & b.low) {
					f(q, 10);
				}
			}

			highType = true;
			q = (p >> 9) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						f(q | HIGH_TYPE, 9);
					}
				} else if (q & b.low) {
					f(q, 9);
				}
			}

			q = p << 1;
			if ((q & INTERIOR) && (q & b.high)) {
				f(q | HIGH_TYPE, -1);
			}
		} else {
			highType = false;
			UL q = p << 11;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						f(q | HIGH_TYPE, -11);
					}
				} else if (q & b.low) {
					f(q, -11);
				}
			}

			highType = false;
			q = p << 10;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						f(q | HIGH_TYPE, -10);
					}
				} else if (q & b.low) {
					f(q, -10);
				}
			}

			highType = false;
			q = p << 9;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						f(q | HIGH_TYPE, -9);
					}
				} else if (q & b.low) {
					f(q, -9);
				}
			}

			q = p >> 1;
			if ((q & INTERIOR) && (q & b.low)) {
				f(q, 1);
			}

			q = p >> 11;
			if ((q & INTERIOR) && (q & b.low)) {
				f(q, 11);
			}

			q = p >> 10;
			if ((q & INTERIOR) && (q & b.low)) {
				f(q, 10);
			}

			q = p >> 9;
			if ((q & INTERIOR) && (q & b.low)) {
				f(q, 9);
			}

			q = p << 1;
			if ((q & INTERIOR) && (q & b.low)) {
				f(q, -1);
			}
		}
	}

	//optimized version of iterateOnNeighbors
	template<class Function>
	constexpr bool anyOfOccupiedNeighbors_(const UL pos, const Board& b, Function f) {
		UL p = pos;
		bool highType = p & HIGH_TYPE;

		//SHIFT[8] = {-11,-10,-9,1,11,10,9,-1};
		if (highType) {
			//remove high type from p else it will be shifted
			p &= NOT_HIGH_TYPE;

			UL q = p << 11;
			if ((q & INTERIOR) && (q & b.high)) {
				if (f(q | HIGH_TYPE, -11)) return true;
			}

			q = p << 10;
			if ((q & INTERIOR) && (q & b.high)) {
				if (f(q | HIGH_TYPE, -10)) return true;
			}

			q = p << 9;
			if ((q & INTERIOR) && (q & b.high)) {
				if (f(q | HIGH_TYPE, -9)) return true;
			}

			q = p >> 1;
			if ((q & INTERIOR) && (q & b.high)) {
				if (f(q | HIGH_TYPE, 1)) return true;
			}

			highType = true;
			q = (p >> 11) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						if (f(q | HIGH_TYPE, 11)) return true;
					}
				} else if (q & b.low) {
					if (f(q, 11)) return true;
				}
			}

			highType = true;
			q = (p >> 10) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						if (f(q | HIGH_TYPE, 10)) return true;
					}
				} else if (q & b.low) {
					if (f(q, 10)) return true;
				}
			}

			highType = true;
			q = (p >> 9) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						if (f(q | HIGH_TYPE, 9)) return true;
					}
				} else if (q & b.low) {
					if (f(q, 9)) return true;
				}
			}

			q = p << 1;
			if ((q & INTERIOR) && (q & b.high)) {
				if (f(q | HIGH_TYPE, -1)) return true;
			}
		} else {
			highType = false;
			UL q = p << 11;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						if (f(q | HIGH_TYPE, -11)) return true;
					}
				} else if (q & b.low) {
					if (f(q, -11)) return true;
				}
			}

			highType = false;
			q = p << 10;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						if (f(q | HIGH_TYPE, -10)) return true;
					}
				} else if (q & b.low) {
					if (f(q, -10)) return true;
				}
			}

			highType = false;
			q = p << 9;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					if (q & b.high) {
						if (f(q | HIGH_TYPE, -9)) return true;
					}
				} else if (q & b.low) {
					if (f(q, -9)) return true;
				}
			}

			q = p >> 1;
			if ((q & INTERIOR) && (q & b.low)) {
				if (f(q, 1)) return true;
			}

			q = p >> 11;
			if ((q & INTERIOR) && (q & b.low)) {
				if (f(q, 11)) return true;
			}

			q = p >> 10;
			if ((q & INTERIOR) && (q & b.low)) {
				if (f(q, 10)) return true;
			}

			q = p >> 9;
			if ((q & INTERIOR) && (q & b.low)) {
				if (f(q, 9)) return true;
			}

			q = p << 1;
			if ((q & INTERIOR) && (q & b.low)) {
				if (f(q, -1)) return true;
			}
		}
		return false;
	}

	//optimized version of iterateOnNeighbors
	template<class Function>
	constexpr void iterateOnNeighbors_(const UL pos, Function f_low, Function f_high) {
		UL p = pos;
		bool highType = p & HIGH_TYPE;

		//SHIFT[8] = {-11,-10,-9,1,11,10,9,-1};
		if (highType) {
			//remove high type from p else it will be shifted
			p &= NOT_HIGH_TYPE;

			UL q = p << 11;
			if (q & INTERIOR) {
				f_high(q | HIGH_TYPE, -11);
			}

			q = p << 10;
			if (q & INTERIOR) {
				f_high(q | HIGH_TYPE, -10);
			}

			q = p << 9;
			if (p & INTERIOR) {
				f_high(q | HIGH_TYPE, -9);
			}

			q = p >> 1;
			if (q & INTERIOR) {
				f_high(q | HIGH_TYPE, 1);
			}

			highType = true;
			q = (p >> 11) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}

			if (q & INTERIOR) {
				if (highType) {
					f_high(q | HIGH_TYPE, 11);
				} else {
					f_low(q, 11);
				}
			}

			highType = true;
			q = (p >> 10) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					f_high(q | HIGH_TYPE, 10);
				} else {
					f_low(q, 10);
				}
			}

			highType = true;
			q = (p >> 9) | HIGH_TYPE;
			if (q & HIGH_BORDER) {
				//border change from high to low
				q <<= 40;
				highType = false;
			}
			if (q & INTERIOR) {
				if (highType) {
					f_high(q | HIGH_TYPE, 9);
				} else {
					f_low(q, 9);
				}
			}

			q = p << 1;
			if (q & INTERIOR) {
				f_high(q | HIGH_TYPE, -1);
			}
		} else {
			highType = false;
			UL q = p << 11;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					f_high(q | HIGH_TYPE, -11);
				} else {
					f_low(q, -11);
				}
			}


			highType = false;
			q = p << 10;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					f_high(q | HIGH_TYPE, -10);
				} else {
					f_low(q, -10);
				}
			}

			highType = false;
			q = p << 9;
			if (q & LOW_BORDER) {
				//border change from low to high
				q >>= 40;
				highType = true;
			}
			if (q & INTERIOR) {
				if (highType) {
					f_high(q | HIGH_TYPE, -9);
				} else {
					f_low(q, -9);
				}
			}

			q = p >> 1;
			if (q & INTERIOR) {
				f_low(q, 1);
			}

			q = p >> 11;
			if (q & INTERIOR) {
				f_low(q, 11);
			}

			q = p >> 10;
			if (q & INTERIOR) {
				f_low(q, 10);
			}

			q = p >> 9;
			if (q & INTERIOR) {
				f_low(q, 9);
			}

			q = p << 1;
			if (q & INTERIOR) {
				f_low(q, -1);
			}
		}
	}

	/**
	 * Neighbor of a position for a given shift
	 * @param pos
	 * @param s
	 * @return the UL intern position value (64 bits with HIGH_TYPE)
	 */
	constexpr UL neighbor(const UL pos, const short s) const {
		UL p = pos;
		bool highType = p & HIGH_TYPE;

		if (highType) {
			//remove high type from p else it will be shifted
			p &= NOT_HIGH_TYPE;
		}

		p = (s < 0 ? p << -s : p >> s);

		if (highType) {
			if (p & HIGH_BORDER) {
				//border change from high to low
				p <<= 40;
				highType = false;
			}
		} else if (p & LOW_BORDER) {
			//border change from low to high
			p >>= 40;
			highType = true;
		}

		if (highType) {
			p |= HIGH_TYPE;
		}

		return p;
	}

	/**
	 * Neighbor of a position for a given shift
	 * @param pos
	 * @param s
	 * @return the UL intern position value (64 bits with HIGH_TYPE)
	 */
	constexpr UL neighbor(const UL pos, const short s, const Board& other, bool& occupied) const {
		UL p = pos;
		bool highType = p & HIGH_TYPE;

		if (highType) {
			//remove high type from p else it will be shifted
			p &= NOT_HIGH_TYPE;
		}

		p = (s < 0 ? p << -s : p >> s);

		if (highType) {
			if (p & HIGH_BORDER) {
				//border change from high to low
				p <<= 40;
				highType = false;
			}
		} else if (p & LOW_BORDER) {
			//border change from low to high
			p >>= 40;
			highType = true;
		}

		if (highType) {
			p |= HIGH_TYPE;
			occupied = p & other.high;
		} else {
			occupied = p & other.low;
		}

		return p;
	}

	/**
	 *
	 * @param p
	 * @return
	 */
	constexpr bool isEmpty(UL p) const {
		return !isOccupied(p);
	}

	/**
	 *
	 * @param p
	 * @return
	 */
	constexpr bool isOccupied(UL p) const {
		return p & HIGH_TYPE ? p & high : p & low;
	}

	/**
	 *
	 * @param p
	 * @return
	 */
	constexpr bool isInBorder(UL p) const {
		return (p & INTERIOR) == 0;
	}

	/**
	 *
	 * @param s
	 * @return
	 */
	constexpr Board& AND(const UL s) {
		if (s & HIGH_TYPE) {
			high &= s;
		} else {
			low &= s;
		}
		return *this;
	}

	/**
	 *
	 * @param s
	 * @return
	 */
	constexpr Board& OR(const UL s) {
		if (s & HIGH_TYPE) {
			high |= (s & ~HIGH_TYPE); // don't copy high bit in high
		} else {
			low |= s;
		}
		return *this;
	}

	constexpr bool operator==(const Board& board) const {
		return !(low ^ board.low) && !(high ^ board.high);
	}

	constexpr bool operator!=(const Board& board) const {
		return !(*this == board);
	}

	/**
	 *
	 * @param f
	 * @param board
	 * @return
	 */
	friend std::ostream& operator<<(std::ostream &f, const Board& board) {
		f << "    a  b  c  d  e  f  g  h\n";
		f << " 0  1  2  3  4  5  6  7  8  9\n";
		f << "  ----------------------------\n";
		for (US i {0}; i < 60; ++i) {
			UL pi = 1L << (59 - i);
			if (board.high & pi) {
				f << " . ";
			} else if (pi & Board::BORDER) {
				f << " 0 ";
			} else {
				f << "   ";
			}
			if ((i+1)%10 == 0) f << "\n";
		}
		for (US i {0}; i < 60; ++i) {
			UL pi = 1L << (59 - i);
			if (board.low & pi) {
				f << " . ";
			} else if (pi & Board::BORDER) {
				f << " 0 ";
			} else {
				f << "   ";
			}
			if ((i+1)%10 == 0) f << "\n";
		}

		f << "\n  --------------------------" << std::endl;
		return f;

	}

	constexpr US count_() const {
		US count = 0;
		iterateOnPositions([&count, this](UL p) {
			if (isOccupied(p)) {
				count++;
			}
			return true;
		});
		return count;
	}

	constexpr US count() const {
		US count = 0;
		UL s = 1L << 11; // valid low & high positions start at 11
		for (UL i {0}; i < 37; ++i) {
			UL p = Board::INTERIOR & s;
			if (p & low) {
				count++;
			}
			if (p & high) {
				count++;
			}
			s <<= 1;
		}
		return count;
	}

	/**
	 * Convert internal format positions to board positions (11=A1,23=B3...)
	 * @param pos
	 * @return
	 */
	constexpr static US convert(UL pos) {
		const UL MASK = 0b11111111;
		UL ps = pos;
		UL re = 0;

		if (pos) {
			if (pos & HIGH_TYPE) {
				//ps &= ~HIGH_TYPE;// remove HIGH type;
				UL mask = MASK << 56;
				ps <<= 15;
				re |= ps & mask;

				//			std::bitset<64> bmask(mask);
				//			std::bitset<64> bps(ps);
				//			std::bitset<64> bre(re);
				//			std::cout << "\nmask=" << bmask;
				//			std::cout << "\nps  =" << bps;
				//			std::cout << "\nre  =" << bre;

				ps <<= 2;
				re |= ps & (mask >>= 8);

				//			bps = ps; bre=re; bmask = mask;
				//			std::cout << "\nmask=" << bmask;
				//			std::cout << "\nps  =" << bps;
				//			std::cout << "\nre  =" << bre;

				ps <<= 2;
				re |= ps & (mask >>= 8);

				//			bps = ps; bre=re; bmask = mask;
				//			std::cout << "\nmask=" << bmask;
				//			std::cout << "\nps  =" << bps;
				//			std::cout << "\nre  =" << bre;

				ps <<= 2;
				re |= ps & (mask >>= 8);
				//			bps = ps; bre=re; bmask = mask;
				//			std::cout << "\nmask=" << bmask;
				//			std::cout << "\nps  =" << bps;
				//			std::cout << "\nre  =" << bre;
			} else {
				UL mask = MASK;
				ps >>= 11;
				re |= ps & mask;
				ps >>= 2;
				re |= ps & (mask <<= 8);
				ps >>= 2;
				re |= ps & (mask <<= 8);
				ps >>= 2;
				re |= ps & (mask <<= 8);
			}
		}
		//		std::bitset<64> x(re);
		//		std::cout << "re=" << x << ", ";

		return toShort(re);
	}

	/**
	 * Convert long positions (64 bits) to board positions (11=A1,23=B3...)
	 * @param p
	 * @return
	 */
	constexpr static US toShort(UL p) {
		US res = 0;
		if (p) {
			UL l = p;
			US k = 0;
			UL pl = l;
			do {
				pl = l;
				l >>= 8;
				k++;
			} while (l);
			//cout << std::format("k={:d}, pl={:d}\n", 8 - k + 1, pl);

			US m = 0;
			do {
				l = pl;
				pl >>= 1;
				m++;
			} while (pl);
			//cout << std::format("m={:d}, l={:d}\n", 8 - m + 1, l);

			res = (8 - m + 1) * 10 + 8 - k + 1;
		}
		return res;
	}


	constexpr static US toLinearPos(UL p) {
		US s = convert(p);
		US y = s / 10;
		US x = s - 10 * y;
		return (y - 1) * 8 + x - 1;
	}

	constexpr void clear() {
		low &= 0;
		high &= 0;
	}

	void set(UL l, UL h) {
		low &= 0;
		high &= 0;
		low |= l;
		high |= h;
	}

	void check(US pos) {
		US u = 99 - pos;
		US y = u / 10;
		US x = u - 10 * y;
		US r = 10 * x + y;
		//xy - yx -
		if (r < 50) {
			low |= 1L << r;
		} else {
			high |= 1L << (r - 40);
		}
	}

	constexpr UL at(US pos) const {
		US u = 99 - pos;
		US y = u / 10;
		US x = u - 10 * y;
		US r = 10 * x + y;
		return 1L << (x < 5 ? r : r - 40);
	}

	constexpr bool is(US pos) const {
		US u = 99 - pos;
		US y = u / 10;
		US x = u - 10 * y;
		US r = 10 * x + y;
		return x < 5 ? low & (1L << r) : high & (1L << (r - 40));
	}

	constexpr bool is(US col, US row) const {
		const US r = 10 * row + col;
		return row > 4 ? low & (1L << (99 - r)) : high & (1L << (59 - r));
	}

};

/**
 * The Othello board
 */
class OthelloBoard final : public std::enable_shared_from_this<OthelloBoard> {
public:
	OthelloBoard() : m_black(), m_white() {
		reset();
	};
	~OthelloBoard() = default;
	OthelloBoard(const OthelloBoard &other) = default;
	OthelloBoard(OthelloBoard &&other) = default;
	OthelloBoard& operator=(const OthelloBoard &other) = default;
	OthelloBoard& operator=(OthelloBoard &&other) = default;

	void reset() {
		m_black.reset(1L << 45, 1L << 14);
		m_white.reset(1L << 44, 1L << 15);
	}

	constexpr US count(bool black) {
		return board(black).count_();
	}

	/**
	 * Load a position. Each value is for black and white board resp. and
	 * can represent a game position in a 64 board positions (UL is 64 bits)
	 * @param black
	 * @param white
	 */
	void load(UL black, UL white);

	/**
	 * @return
	 */
	constexpr bool isFull() const {
		return ((m_black.low | m_white.low) == Board::FULL) &&
			   (((m_black.high | m_white.high) == Board::FULL) | Board::HIGH_TYPE);
	}

	/**
	 *
	 * @param p
	 * @return
	 */
	constexpr bool isEmpty(UL p) const {
		return m_black.isEmpty(p) & m_white.isEmpty(p);
	}

	constexpr bool operator==(const OthelloBoard& board) const {
		return (m_black == board.m_black) && (m_white == board.m_white);
	}

	constexpr bool operator!=(const OthelloBoard& board) const {
		return !(*this == board);
	}

	/**
	 * Convert standard board positions (A1=11,...) to intern long positions (64 bits)
	 * @param pos
	 * @return
	 */
	constexpr static UL toLong(US pos) {
		US u = 99 - pos;
		US y = u / 10;
		US x = u - 10 * y;
		US r = 10 * x + y;
		return x < 5 ? 1L << r : (1L << (r - 40)) | Board::HIGH_TYPE;
	}

	/**
	 * Convert long positions (64 bits) to standard board positions (A1=11,...)
	 * @param pos
	 * @return
	 */
	constexpr static US toStandard(UL pos) {
		return Board::convert(pos);
	}

	constexpr Board& board(bool black) {
		return black ? m_black : m_white;
	}

	/**
	 * Compute the so called 'border' which is the number of outside disks
	 * @param black
	 * @return
	 */
	US border(bool black);
	US border_(bool black);
	US border__(bool black);

	/**
	 * The mobility is the number of available moves
	 * @param black the side to compute
	 * @return
	 */
	US mobility(bool black);

	/**
	 * List of all available moves starting from this board for a given player (black or not)
	 * @param black the side to compute
	 * @return
	 */
	std::vector<UL> mobilityList(bool black);

	/**
	 * List of all available moves starting from this board for a given player (black or not).
	 * If present, the corners positions will be in the first positions of the list
	 * @param black the side to compute
	 * @return
	 */
	std::vector<UL> mobilityListCornersFirst(bool black);
	std::vector<UL> mobilityListCornersFirst_(bool black);

	/**
	 * Compute the number of corner
	 * @param black
	 * @return
	 */
	constexpr US corner(bool black) {
		Board& board = this->board(black);
		return 	(Board::CORNER_TOP_LEFT & board.high ? 1 : 0) +
				(Board::CORNER_TOP_RIGHT & board.high ? 1 : 0) +
				(Board::CORNER_BOTTOM_LEFT & board.low ? 1 : 0) +
				(Board::CORNER_BOTTOM_RIGHT & board.low ? 1 : 0);
	}

	/**
	 * Check if there is any available move
	 * @param black
	 * @return
	 */
	bool hasMove(bool black);

	/**
	 * Flips disk(s)
	 * @param p intern format position
	 * @param black
	 * @return
	 */
	US flip(UL p, bool black);

	/**
	 * Test flip disk(s)
	 * @param p intern format position
	 * @param black
	 * @return
	 */
	US testflip(UL p, bool black);
	US testflip_noEmptyCheck(UL p, bool black);
	US canFlip_noEmptyCheck(UL p, bool black);


	Board& blackBoard();
	Board& whiteBoard();


	/**
	 *
	 * @param p
	 * @param black
	 * @return
	 */
	US play(US p, bool black);

	bool canPlay(US p, bool black);



	friend std::ostream& operator<<(std::ostream &f, OthelloBoard& board);

private:
	Board m_black;
	Board m_white;

};

} /* namespace othello */

