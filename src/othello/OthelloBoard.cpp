/*
 * OthelloBoard.cpp
 *
 *  Created on: Jun 5, 2024
 *      Author: sandro
 */

#include <format>

#include "othello/OthelloBoard.h"

namespace othello {

const std::vector<UL> Board::POS_({1L << 11, Board::HIGH_TYPE | (1L << 11), 1L << 12, Board::HIGH_TYPE | (1L << 12), 1L << 13, Board::HIGH_TYPE | (1L << 13), 1L << 14, Board::HIGH_TYPE | (1L << 14), 1L << 15, Board::HIGH_TYPE | (1L << 15), 1L << 16, Board::HIGH_TYPE | (1L << 16), 1L << 17, Board::HIGH_TYPE | (1L << 17), 1L << 18, Board::HIGH_TYPE | (1L << 18), 1L << 21, Board::HIGH_TYPE | (1L << 21), 1L << 22, Board::HIGH_TYPE | (1L << 22), 1L << 23, Board::HIGH_TYPE | (1L << 23), 1L << 24, Board::HIGH_TYPE | (1L << 24), 1L << 25, Board::HIGH_TYPE | (1L << 25), 1L << 26, Board::HIGH_TYPE | (1L << 26), 1L << 27, Board::HIGH_TYPE | (1L << 27), 1L << 28, Board::HIGH_TYPE | (1L << 28), 1L << 31, Board::HIGH_TYPE | (1L << 31), 1L << 32, Board::HIGH_TYPE | (1L << 32), 1L << 33, Board::HIGH_TYPE | (1L << 33), 1L << 34, Board::HIGH_TYPE | (1L << 34), 1L << 35, Board::HIGH_TYPE | (1L << 35), 1L << 36, Board::HIGH_TYPE | (1L << 36), 1L << 37, Board::HIGH_TYPE | (1L << 37), 1L << 38, Board::HIGH_TYPE | (1L << 38), 1L << 41, Board::HIGH_TYPE | (1L << 41), 1L << 42, Board::HIGH_TYPE | (1L << 42), 1L << 43, Board::HIGH_TYPE | (1L << 43), 1L << 44, Board::HIGH_TYPE | (1L << 44), 1L << 45, Board::HIGH_TYPE | (1L << 45), 1L << 46, Board::HIGH_TYPE | (1L << 46), 1L << 47, Board::HIGH_TYPE | (1L << 47), 1L << 48, Board::HIGH_TYPE | (1L << 48)});
const unsigned int long Board::NEIGHBORS_[] =
{
		1L << 11, 3,       1L<<12,       1L<<21,1L<<22,
		1L << 12, 5,1L<<11,1L<<13,1L<<21,1L<<22,1L<<23,
		1L << 13, 5,1L<<12,1L<<14,1L<<22,1L<<23,1L<<24,
		1L << 14, 5,1L<<13,1L<<15,1L<<23,1L<<24,1L<<25,
		1L << 15, 5,1L<<14,1L<<16,1L<<24,1L<<25,1L<<26,
		1L << 16, 5,1L<<15,1L<<17,1L<<25,1L<<26,1L<<27,
		1L << 17, 5,1L<<16,1L<<18,1L<<26,1L<<27,1L<<28,
		1L << 18, 3,1L<<17       ,1L<<27,1L<<28,

		1L << 21, 5,       1L<<11,1L<<12,       1L<<22,       1L<<31,1L<<32,
		1L << 22, 8,1L<<11,1L<<12,1L<<13,1L<<21,1L<<23,1L<<31,1L<<32,1L<<33,
		1L << 23, 8,1L<<12,1L<<13,1L<<14,1L<<22,1L<<24,1L<<32,1L<<33,1L<<34,
		1L << 24, 8,1L<<13,1L<<14,1L<<15,1L<<23,1L<<25,1L<<33,1L<<34,1L<<35,
		1L << 25, 8,1L<<14,1L<<15,1L<<16,1L<<24,1L<<26,1L<<34,1L<<35,1L<<36,
		1L << 26, 8,1L<<15,1L<<16,1L<<17,1L<<25,1L<<27,1L<<35,1L<<36,1L<<37,
		1L << 27, 8,1L<<16,1L<<17,1L<<18,1L<<26,1L<<28,1L<<36,1L<<37,1L<<38,
		1L << 28, 5,1L<<17,1L<<18,       1L<<27,       1L<<37,1L<<38,

		1L << 31, 5,       1L<<21,1L<<22,       1L<<32,       1L<<41,1L<<42,
		1L << 32, 8,1L<<21,1L<<22,1L<<23,1L<<31,1L<<33,1L<<41,1L<<42,1L<<43,
		1L << 33, 8,1L<<22,1L<<23,1L<<24,1L<<32,1L<<34,1L<<42,1L<<43,1L<<44,
		1L << 34, 8,1L<<23,1L<<24,1L<<25,1L<<33,1L<<35,1L<<43,1L<<44,1L<<45,
		1L << 35, 8,1L<<24,1L<<25,1L<<26,1L<<34,1L<<36,1L<<44,1L<<45,1L<<46,
		1L << 36, 8,1L<<25,1L<<26,1L<<27,1L<<35,1L<<37,1L<<45,1L<<46,1L<<47,
		1L << 37, 8,1L<<26,1L<<27,1L<<28,1L<<36,1L<<38,1L<<46,1L<<47,1L<<48,
		1L << 38, 5,1L<<27,1L<<28,       1L<<37,       1L<<47,1L<<48,

		1L << 41, 5,       1L<<31,1L<<32,       1L<<42,                          Board::HIGH_TYPE|(1L<<11),Board::HIGH_TYPE|(1L<<12),
		1L << 42, 8,1L<<31,1L<<32,1L<<33,1L<<41,1L<<43,Board::HIGH_TYPE|(1L<<11),Board::HIGH_TYPE|(1L<<12),Board::HIGH_TYPE|(1L<<13),
		1L << 43, 8,1L<<32,1L<<33,1L<<34,1L<<42,1L<<44,Board::HIGH_TYPE|(1L<<12),Board::HIGH_TYPE|(1L<<13),Board::HIGH_TYPE|(1L<<14),
		1L << 44, 8,1L<<33,1L<<34,1L<<35,1L<<43,1L<<45,Board::HIGH_TYPE|(1L<<13),Board::HIGH_TYPE|(1L<<14),Board::HIGH_TYPE|(1L<<15),
		1L << 45, 8,1L<<34,1L<<35,1L<<36,1L<<44,1L<<46,Board::HIGH_TYPE|(1L<<14),Board::HIGH_TYPE|(1L<<15),Board::HIGH_TYPE|(1L<<16),
		1L << 46, 8,1L<<35,1L<<36,1L<<37,1L<<45,1L<<47,Board::HIGH_TYPE|(1L<<15),Board::HIGH_TYPE|(1L<<16),Board::HIGH_TYPE|(1L<<17),
		1L << 47, 8,1L<<36,1L<<37,1L<<38,1L<<46,1L<<48,Board::HIGH_TYPE|(1L<<16),Board::HIGH_TYPE|(1L<<17),Board::HIGH_TYPE|(1L<<18),
		1L << 48, 5,1L<<37,1L<<38,       1L<<47,       Board::HIGH_TYPE|(1L<<17),Board::HIGH_TYPE|(1L<<18),
//---
		Board::HIGH_TYPE|(1L << 11), 5,       1L<<41,1L<<42,                          Board::HIGH_TYPE|(1L<<12),                          Board::HIGH_TYPE|(1L<<21),Board::HIGH_TYPE|(1L<<22),
		Board::HIGH_TYPE|(1L << 12), 8,1L<<41,1L<<42,1L<<43,Board::HIGH_TYPE|(1L<<11),Board::HIGH_TYPE|(1L<<13),Board::HIGH_TYPE|(1L<<21),Board::HIGH_TYPE|(1L<<22),Board::HIGH_TYPE|(1L<<23),
		Board::HIGH_TYPE|(1L << 13), 8,1L<<42,1L<<43,1L<<44,Board::HIGH_TYPE|(1L<<12),Board::HIGH_TYPE|(1L<<14),Board::HIGH_TYPE|(1L<<22),Board::HIGH_TYPE|(1L<<23),Board::HIGH_TYPE|(1L<<24),
		Board::HIGH_TYPE|(1L << 14), 8,1L<<43,1L<<44,1L<<45,Board::HIGH_TYPE|(1L<<13),Board::HIGH_TYPE|(1L<<15),Board::HIGH_TYPE|(1L<<23),Board::HIGH_TYPE|(1L<<24),Board::HIGH_TYPE|(1L<<25),
		Board::HIGH_TYPE|(1L << 15), 8,1L<<44,1L<<45,1L<<46,Board::HIGH_TYPE|(1L<<14),Board::HIGH_TYPE|(1L<<16),Board::HIGH_TYPE|(1L<<24),Board::HIGH_TYPE|(1L<<25),Board::HIGH_TYPE|(1L<<26),
		Board::HIGH_TYPE|(1L << 16), 8,1L<<45,1L<<46,1L<<47,Board::HIGH_TYPE|(1L<<15),Board::HIGH_TYPE|(1L<<17),Board::HIGH_TYPE|(1L<<25),Board::HIGH_TYPE|(1L<<26),Board::HIGH_TYPE|(1L<<27),
		Board::HIGH_TYPE|(1L << 17), 8,1L<<46,1L<<47,1L<<48,Board::HIGH_TYPE|(1L<<16),Board::HIGH_TYPE|(1L<<18),Board::HIGH_TYPE|(1L<<26),Board::HIGH_TYPE|(1L<<27),Board::HIGH_TYPE|(1L<<28),
		Board::HIGH_TYPE|(1L << 18), 5,1L<<47,1L<<48,       Board::HIGH_TYPE|(1L<<17),                          Board::HIGH_TYPE|(1L<<27),Board::HIGH_TYPE|(1L<<28),

		Board::HIGH_TYPE|(1L << 21), 5,                           Board::HIGH_TYPE|(1L<<11),Board::HIGH_TYPE|(1L<<12),                          Board::HIGH_TYPE|(1L<<22),                          Board::HIGH_TYPE|(1L<<31),Board::HIGH_TYPE|(1L<<32),
		Board::HIGH_TYPE|(1L << 22), 8, Board::HIGH_TYPE|(1L<<11),Board::HIGH_TYPE|(1L<<12),Board::HIGH_TYPE|(1L<<13),Board::HIGH_TYPE|(1L<<21),Board::HIGH_TYPE|(1L<<23),Board::HIGH_TYPE|(1L<<31),Board::HIGH_TYPE|(1L<<32),Board::HIGH_TYPE|(1L<<33),
		Board::HIGH_TYPE|(1L << 23), 8, Board::HIGH_TYPE|(1L<<12),Board::HIGH_TYPE|(1L<<13),Board::HIGH_TYPE|(1L<<14),Board::HIGH_TYPE|(1L<<22),Board::HIGH_TYPE|(1L<<24),Board::HIGH_TYPE|(1L<<32),Board::HIGH_TYPE|(1L<<33),Board::HIGH_TYPE|(1L<<34),
		Board::HIGH_TYPE|(1L << 24), 8, Board::HIGH_TYPE|(1L<<13),Board::HIGH_TYPE|(1L<<14),Board::HIGH_TYPE|(1L<<15),Board::HIGH_TYPE|(1L<<23),Board::HIGH_TYPE|(1L<<25),Board::HIGH_TYPE|(1L<<33),Board::HIGH_TYPE|(1L<<34),Board::HIGH_TYPE|(1L<<35),
		Board::HIGH_TYPE|(1L << 25), 8, Board::HIGH_TYPE|(1L<<14),Board::HIGH_TYPE|(1L<<15),Board::HIGH_TYPE|(1L<<16),Board::HIGH_TYPE|(1L<<24),Board::HIGH_TYPE|(1L<<26),Board::HIGH_TYPE|(1L<<34),Board::HIGH_TYPE|(1L<<35),Board::HIGH_TYPE|(1L<<36),
		Board::HIGH_TYPE|(1L << 26), 8, Board::HIGH_TYPE|(1L<<15),Board::HIGH_TYPE|(1L<<16),Board::HIGH_TYPE|(1L<<17),Board::HIGH_TYPE|(1L<<25),Board::HIGH_TYPE|(1L<<27),Board::HIGH_TYPE|(1L<<35),Board::HIGH_TYPE|(1L<<36),Board::HIGH_TYPE|(1L<<37),
		Board::HIGH_TYPE|(1L << 27), 8, Board::HIGH_TYPE|(1L<<16),Board::HIGH_TYPE|(1L<<17),Board::HIGH_TYPE|(1L<<18),Board::HIGH_TYPE|(1L<<26),Board::HIGH_TYPE|(1L<<28),Board::HIGH_TYPE|(1L<<36),Board::HIGH_TYPE|(1L<<37),Board::HIGH_TYPE|(1L<<38),
		Board::HIGH_TYPE|(1L << 28), 5, Board::HIGH_TYPE|(1L<<17),Board::HIGH_TYPE|(1L<<18),                          Board::HIGH_TYPE|(1L<<27),                          Board::HIGH_TYPE|(1L<<37),Board::HIGH_TYPE|(1L<<38),

		Board::HIGH_TYPE|(1L << 31), 5,                           Board::HIGH_TYPE|(1L<<21),Board::HIGH_TYPE|(1L<<22),                          Board::HIGH_TYPE|(1L<<32),                          Board::HIGH_TYPE|(1L<<41),Board::HIGH_TYPE|(1L<<42),
		Board::HIGH_TYPE|(1L << 32), 8, Board::HIGH_TYPE|(1L<<21),Board::HIGH_TYPE|(1L<<22),Board::HIGH_TYPE|(1L<<23),Board::HIGH_TYPE|(1L<<31),Board::HIGH_TYPE|(1L<<33),Board::HIGH_TYPE|(1L<<41),Board::HIGH_TYPE|(1L<<42),Board::HIGH_TYPE|(1L<<43),
		Board::HIGH_TYPE|(1L << 33), 8, Board::HIGH_TYPE|(1L<<22),Board::HIGH_TYPE|(1L<<23),Board::HIGH_TYPE|(1L<<24),Board::HIGH_TYPE|(1L<<32),Board::HIGH_TYPE|(1L<<34),Board::HIGH_TYPE|(1L<<42),Board::HIGH_TYPE|(1L<<43),Board::HIGH_TYPE|(1L<<44),
		Board::HIGH_TYPE|(1L << 34), 8, Board::HIGH_TYPE|(1L<<23),Board::HIGH_TYPE|(1L<<24),Board::HIGH_TYPE|(1L<<25),Board::HIGH_TYPE|(1L<<33),Board::HIGH_TYPE|(1L<<35),Board::HIGH_TYPE|(1L<<43),Board::HIGH_TYPE|(1L<<44),Board::HIGH_TYPE|(1L<<45),
		Board::HIGH_TYPE|(1L << 35), 8, Board::HIGH_TYPE|(1L<<24),Board::HIGH_TYPE|(1L<<25),Board::HIGH_TYPE|(1L<<26),Board::HIGH_TYPE|(1L<<34),Board::HIGH_TYPE|(1L<<36),Board::HIGH_TYPE|(1L<<44),Board::HIGH_TYPE|(1L<<45),Board::HIGH_TYPE|(1L<<46),
		Board::HIGH_TYPE|(1L << 36), 8, Board::HIGH_TYPE|(1L<<25),Board::HIGH_TYPE|(1L<<26),Board::HIGH_TYPE|(1L<<27),Board::HIGH_TYPE|(1L<<35),Board::HIGH_TYPE|(1L<<37),Board::HIGH_TYPE|(1L<<45),Board::HIGH_TYPE|(1L<<46),Board::HIGH_TYPE|(1L<<47),
		Board::HIGH_TYPE|(1L << 37), 8, Board::HIGH_TYPE|(1L<<26),Board::HIGH_TYPE|(1L<<27),Board::HIGH_TYPE|(1L<<28),Board::HIGH_TYPE|(1L<<36),Board::HIGH_TYPE|(1L<<38),Board::HIGH_TYPE|(1L<<46),Board::HIGH_TYPE|(1L<<47),Board::HIGH_TYPE|(1L<<48),
		Board::HIGH_TYPE|(1L << 38), 5, Board::HIGH_TYPE|(1L<<27),Board::HIGH_TYPE|(1L<<28),                          Board::HIGH_TYPE|(1L<<37),                          Board::HIGH_TYPE|(1L<<47),Board::HIGH_TYPE|(1L<<48),

		Board::HIGH_TYPE|(1L << 41), 3,                           Board::HIGH_TYPE|(1L<<31),Board::HIGH_TYPE|(1L<<32),                          Board::HIGH_TYPE|(1L<<42),
		Board::HIGH_TYPE|(1L << 42), 5, Board::HIGH_TYPE|(1L<<31),Board::HIGH_TYPE|(1L<<32),Board::HIGH_TYPE|(1L<<33),Board::HIGH_TYPE|(1L<<41),Board::HIGH_TYPE|(1L<<43),
		Board::HIGH_TYPE|(1L << 43), 5, Board::HIGH_TYPE|(1L<<32),Board::HIGH_TYPE|(1L<<33),Board::HIGH_TYPE|(1L<<34),Board::HIGH_TYPE|(1L<<42),Board::HIGH_TYPE|(1L<<44),
		Board::HIGH_TYPE|(1L << 44), 5, Board::HIGH_TYPE|(1L<<33),Board::HIGH_TYPE|(1L<<34),Board::HIGH_TYPE|(1L<<35),Board::HIGH_TYPE|(1L<<43),Board::HIGH_TYPE|(1L<<45),
		Board::HIGH_TYPE|(1L << 45), 5, Board::HIGH_TYPE|(1L<<34),Board::HIGH_TYPE|(1L<<35),Board::HIGH_TYPE|(1L<<36),Board::HIGH_TYPE|(1L<<44),Board::HIGH_TYPE|(1L<<46),
		Board::HIGH_TYPE|(1L << 46), 5, Board::HIGH_TYPE|(1L<<35),Board::HIGH_TYPE|(1L<<36),Board::HIGH_TYPE|(1L<<37),Board::HIGH_TYPE|(1L<<45),Board::HIGH_TYPE|(1L<<47),
		Board::HIGH_TYPE|(1L << 47), 5, Board::HIGH_TYPE|(1L<<36),Board::HIGH_TYPE|(1L<<37),Board::HIGH_TYPE|(1L<<38),Board::HIGH_TYPE|(1L<<46),Board::HIGH_TYPE|(1L<<48),
		Board::HIGH_TYPE|(1L << 48), 3, Board::HIGH_TYPE|(1L<<37),Board::HIGH_TYPE|(1L<<38),                          Board::HIGH_TYPE|(1L<<47)
};
//const unsigned int long Board::NEIGHBORS__[] =
//{
//		1L << 11, 1L<<12|       1L<<21|1L<<22,0,
//		1L << 12, 1L<<11|1L<<13|1L<<21|1L<<22|1L<<23,0,
//		1L << 13, 1L<<12|1L<<14|1L<<22|1L<<23|1L<<24,0,
//		1L << 14, 1L<<13|1L<<15|1L<<23|1L<<24|1L<<25,0,
//		1L << 15, 1L<<14|1L<<16|1L<<24|1L<<25|1L<<26,0,
//		1L << 16, 1L<<15|1L<<17|1L<<25|1L<<26|1L<<27,0,
//		1L << 17, 1L<<16|1L<<18|1L<<26|1L<<27|1L<<28,0,
//		1L << 18, 1L<<17|       1L<<27|1L<<28,0,
//
//		1L << 21,        1L<<11|1L<<12|       1L<<22|       1L<<31|1L<<32,0,
//		1L << 22, 1L<<11|1L<<12|1L<<13|1L<<21|1L<<23|1L<<31|1L<<32|1L<<33,0,
//		1L << 23, 1L<<12|1L<<13|1L<<14|1L<<22|1L<<24|1L<<32|1L<<33|1L<<34,0,
//		1L << 24, 1L<<13|1L<<14|1L<<15|1L<<23|1L<<25|1L<<33|1L<<34|1L<<35,0,
//		1L << 25, 1L<<14|1L<<15|1L<<16|1L<<24|1L<<26|1L<<34|1L<<35|1L<<36,0,
//		1L << 26, 1L<<15|1L<<16|1L<<17|1L<<25|1L<<27|1L<<35|1L<<36|1L<<37,0,
//		1L << 27, 1L<<16|1L<<17|1L<<18|1L<<26|1L<<28|1L<<36|1L<<37|1L<<38,0,
//		1L << 28, 1L<<17|1L<<18|       1L<<27|       1L<<37|1L<<38,0,
//
//		1L << 31,        1L<<21|1L<<22|       1L<<32|       1L<<41|1L<<42,0,
//		1L << 32, 1L<<21|1L<<22|1L<<23|1L<<31|1L<<33|1L<<41|1L<<42|1L<<43,0,
//		1L << 33, 1L<<22|1L<<23|1L<<24|1L<<32|1L<<34|1L<<42|1L<<43|1L<<44,0,
//		1L << 34, 1L<<23|1L<<24|1L<<25|1L<<33|1L<<35|1L<<43|1L<<44|1L<<45,0,
//		1L << 35, 1L<<24|1L<<25|1L<<26|1L<<34|1L<<36|1L<<44|1L<<45|1L<<46,0,
//		1L << 36, 1L<<25|1L<<26|1L<<27|1L<<35|1L<<37|1L<<45|1L<<46|1L<<47,0,
//		1L << 37, 1L<<26|1L<<27|1L<<28|1L<<36|1L<<38|1L<<46|1L<<47|1L<<48,0,
//		1L << 38, 1L<<27|1L<<28|       1L<<37|       1L<<47|1L<<48,0,
//
//		1L << 41,        1L<<31|1L<<32|       1L<<42,                          Board::HIGH_TYPE|(1L<<11)|Board::HIGH_TYPE|(1L<<12),
//		1L << 42, 1L<<31|1L<<32|1L<<33|1L<<41|1L<<43,Board::HIGH_TYPE|(1L<<11)|Board::HIGH_TYPE|(1L<<12)|Board::HIGH_TYPE|(1L<<13),
//		1L << 43, 1L<<32|1L<<33|1L<<34|1L<<42|1L<<44,Board::HIGH_TYPE|(1L<<12)|Board::HIGH_TYPE|(1L<<13)|Board::HIGH_TYPE|(1L<<14),
//		1L << 44, 1L<<33|1L<<34|1L<<35|1L<<43|1L<<45,Board::HIGH_TYPE|(1L<<13)|Board::HIGH_TYPE|(1L<<14)|Board::HIGH_TYPE|(1L<<15),
//		1L << 45, 1L<<34|1L<<35|1L<<36|1L<<44|1L<<46,Board::HIGH_TYPE|(1L<<14)|Board::HIGH_TYPE|(1L<<15)|Board::HIGH_TYPE|(1L<<16),
//		1L << 46, 1L<<35|1L<<36|1L<<37|1L<<45|1L<<47,Board::HIGH_TYPE|(1L<<15)|Board::HIGH_TYPE|(1L<<16)|Board::HIGH_TYPE|(1L<<17),
//		1L << 47, 1L<<36|1L<<37|1L<<38|1L<<46|1L<<48,Board::HIGH_TYPE|(1L<<16)|Board::HIGH_TYPE|(1L<<17)|Board::HIGH_TYPE|(1L<<18),
//		1L << 48, 1L<<37|1L<<38|       1L<<47,       Board::HIGH_TYPE|(1L<<17)|Board::HIGH_TYPE|(1L<<18),
////--
//		Board::HIGH_TYPE|(1L << 11),        1L<<41|1L<<42,                          Board::HIGH_TYPE|(1L<<12)|                          Board::HIGH_TYPE|(1L<<21)|Board::HIGH_TYPE|(1L<<22),
//		Board::HIGH_TYPE|(1L << 12), 1L<<41|1L<<42|1L<<43,Board::HIGH_TYPE|(1L<<11)|Board::HIGH_TYPE|(1L<<13)|Board::HIGH_TYPE|(1L<<21)|Board::HIGH_TYPE|(1L<<22)|Board::HIGH_TYPE|(1L<<23),
//		Board::HIGH_TYPE|(1L << 13), 1L<<42|1L<<43|1L<<44,Board::HIGH_TYPE|(1L<<12)|Board::HIGH_TYPE|(1L<<14)|Board::HIGH_TYPE|(1L<<22)|Board::HIGH_TYPE|(1L<<23)|Board::HIGH_TYPE|(1L<<24),
//		Board::HIGH_TYPE|(1L << 14), 1L<<43|1L<<44|1L<<45,Board::HIGH_TYPE|(1L<<13)|Board::HIGH_TYPE|(1L<<15)|Board::HIGH_TYPE|(1L<<23)|Board::HIGH_TYPE|(1L<<24)|Board::HIGH_TYPE|(1L<<25),
//		Board::HIGH_TYPE|(1L << 15), 1L<<44|1L<<45|1L<<46,Board::HIGH_TYPE|(1L<<14)|Board::HIGH_TYPE|(1L<<16)|Board::HIGH_TYPE|(1L<<24)|Board::HIGH_TYPE|(1L<<25)|Board::HIGH_TYPE|(1L<<26),
//		Board::HIGH_TYPE|(1L << 16), 1L<<45|1L<<46|1L<<47,Board::HIGH_TYPE|(1L<<15)|Board::HIGH_TYPE|(1L<<17)|Board::HIGH_TYPE|(1L<<25)|Board::HIGH_TYPE|(1L<<26)|Board::HIGH_TYPE|(1L<<27),
//		Board::HIGH_TYPE|(1L << 17), 1L<<46|1L<<47|1L<<48,Board::HIGH_TYPE|(1L<<16)|Board::HIGH_TYPE|(1L<<18)|Board::HIGH_TYPE|(1L<<26)|Board::HIGH_TYPE|(1L<<27)|Board::HIGH_TYPE|(1L<<28),
//		Board::HIGH_TYPE|(1L << 18), 1L<<47|1L<<48,       Board::HIGH_TYPE|(1L<<17)|                          Board::HIGH_TYPE|(1L<<27)|Board::HIGH_TYPE|(1L<<28),
//
//		Board::HIGH_TYPE|(1L << 21),0,                            Board::HIGH_TYPE|(1L<<11)|Board::HIGH_TYPE|(1L<<12)|                          Board::HIGH_TYPE|(1L<<22)|                          Board::HIGH_TYPE|(1L<<31)|Board::HIGH_TYPE|(1L<<32),
//		Board::HIGH_TYPE|(1L << 22),0, Board::HIGH_TYPE|(1L<<11)|Board::HIGH_TYPE|(1L<<12)|Board::HIGH_TYPE|(1L<<13)|Board::HIGH_TYPE|(1L<<21)|Board::HIGH_TYPE|(1L<<23)|Board::HIGH_TYPE|(1L<<31)|Board::HIGH_TYPE|(1L<<32)|Board::HIGH_TYPE|(1L<<33),
//		Board::HIGH_TYPE|(1L << 23),0, Board::HIGH_TYPE|(1L<<12)|Board::HIGH_TYPE|(1L<<13)|Board::HIGH_TYPE|(1L<<14)|Board::HIGH_TYPE|(1L<<22)|Board::HIGH_TYPE|(1L<<24)|Board::HIGH_TYPE|(1L<<32)|Board::HIGH_TYPE|(1L<<33)|Board::HIGH_TYPE|(1L<<34),
//		Board::HIGH_TYPE|(1L << 24),0, Board::HIGH_TYPE|(1L<<13)|Board::HIGH_TYPE|(1L<<14)|Board::HIGH_TYPE|(1L<<15)|Board::HIGH_TYPE|(1L<<23)|Board::HIGH_TYPE|(1L<<25)|Board::HIGH_TYPE|(1L<<33)|Board::HIGH_TYPE|(1L<<34)|Board::HIGH_TYPE|(1L<<35),
//		Board::HIGH_TYPE|(1L << 25),0, Board::HIGH_TYPE|(1L<<14)|Board::HIGH_TYPE|(1L<<15)|Board::HIGH_TYPE|(1L<<16)|Board::HIGH_TYPE|(1L<<24)|Board::HIGH_TYPE|(1L<<26)|Board::HIGH_TYPE|(1L<<34)|Board::HIGH_TYPE|(1L<<35)|Board::HIGH_TYPE|(1L<<36),
//		Board::HIGH_TYPE|(1L << 26),0, Board::HIGH_TYPE|(1L<<15)|Board::HIGH_TYPE|(1L<<16)|Board::HIGH_TYPE|(1L<<17)|Board::HIGH_TYPE|(1L<<25)|Board::HIGH_TYPE|(1L<<27)|Board::HIGH_TYPE|(1L<<35)|Board::HIGH_TYPE|(1L<<36)|Board::HIGH_TYPE|(1L<<37),
//		Board::HIGH_TYPE|(1L << 27),0, Board::HIGH_TYPE|(1L<<16)|Board::HIGH_TYPE|(1L<<17)|Board::HIGH_TYPE|(1L<<18)|Board::HIGH_TYPE|(1L<<26)|Board::HIGH_TYPE|(1L<<28)|Board::HIGH_TYPE|(1L<<36)|Board::HIGH_TYPE|(1L<<37)|Board::HIGH_TYPE|(1L<<38),
//		Board::HIGH_TYPE|(1L << 28),0, Board::HIGH_TYPE|(1L<<17)|Board::HIGH_TYPE|(1L<<18)|                          Board::HIGH_TYPE|(1L<<27)|                          Board::HIGH_TYPE|(1L<<37)|Board::HIGH_TYPE|(1L<<38),
//
//		Board::HIGH_TYPE|(1L << 31),0,                           Board::HIGH_TYPE|(1L<<21)|Board::HIGH_TYPE|(1L<<22)|                          Board::HIGH_TYPE|(1L<<32)|                          Board::HIGH_TYPE|(1L<<41)|Board::HIGH_TYPE|(1L<<42),
//		Board::HIGH_TYPE|(1L << 32),0, Board::HIGH_TYPE|(1L<<21)|Board::HIGH_TYPE|(1L<<22)|Board::HIGH_TYPE|(1L<<23)|Board::HIGH_TYPE|(1L<<31)|Board::HIGH_TYPE|(1L<<33)|Board::HIGH_TYPE|(1L<<41)|Board::HIGH_TYPE|(1L<<42)|Board::HIGH_TYPE|(1L<<43),
//		Board::HIGH_TYPE|(1L << 33),0, Board::HIGH_TYPE|(1L<<22)|Board::HIGH_TYPE|(1L<<23)|Board::HIGH_TYPE|(1L<<24)|Board::HIGH_TYPE|(1L<<32)|Board::HIGH_TYPE|(1L<<34)|Board::HIGH_TYPE|(1L<<42)|Board::HIGH_TYPE|(1L<<43)|Board::HIGH_TYPE|(1L<<44),
//		Board::HIGH_TYPE|(1L << 34),0, Board::HIGH_TYPE|(1L<<23)|Board::HIGH_TYPE|(1L<<24)|Board::HIGH_TYPE|(1L<<25)|Board::HIGH_TYPE|(1L<<33)|Board::HIGH_TYPE|(1L<<35)|Board::HIGH_TYPE|(1L<<43)|Board::HIGH_TYPE|(1L<<44)|Board::HIGH_TYPE|(1L<<45),
//		Board::HIGH_TYPE|(1L << 35),0, Board::HIGH_TYPE|(1L<<24)|Board::HIGH_TYPE|(1L<<25)|Board::HIGH_TYPE|(1L<<26)|Board::HIGH_TYPE|(1L<<34)|Board::HIGH_TYPE|(1L<<36)|Board::HIGH_TYPE|(1L<<44)|Board::HIGH_TYPE|(1L<<45)|Board::HIGH_TYPE|(1L<<46),
//		Board::HIGH_TYPE|(1L << 36),0, Board::HIGH_TYPE|(1L<<25)|Board::HIGH_TYPE|(1L<<26)|Board::HIGH_TYPE|(1L<<27)|Board::HIGH_TYPE|(1L<<35)|Board::HIGH_TYPE|(1L<<37)|Board::HIGH_TYPE|(1L<<45)|Board::HIGH_TYPE|(1L<<46)|Board::HIGH_TYPE|(1L<<47),
//		Board::HIGH_TYPE|(1L << 37),0, Board::HIGH_TYPE|(1L<<26)|Board::HIGH_TYPE|(1L<<27)|Board::HIGH_TYPE|(1L<<28)|Board::HIGH_TYPE|(1L<<36)|Board::HIGH_TYPE|(1L<<38)|Board::HIGH_TYPE|(1L<<46)|Board::HIGH_TYPE|(1L<<47)|Board::HIGH_TYPE|(1L<<48),
//		Board::HIGH_TYPE|(1L << 38),0, Board::HIGH_TYPE|(1L<<27)|Board::HIGH_TYPE|(1L<<28)|                          Board::HIGH_TYPE|(1L<<37)|                          Board::HIGH_TYPE|(1L<<47)|Board::HIGH_TYPE|(1L<<48),
//
//		Board::HIGH_TYPE|(1L << 41),0,                           Board::HIGH_TYPE|(1L<<31)|Board::HIGH_TYPE|(1L<<32)|                          Board::HIGH_TYPE|(1L<<42),
//		Board::HIGH_TYPE|(1L << 42),0, Board::HIGH_TYPE|(1L<<31)|Board::HIGH_TYPE|(1L<<32)|Board::HIGH_TYPE|(1L<<33)|Board::HIGH_TYPE|(1L<<41)|Board::HIGH_TYPE|(1L<<43),
//		Board::HIGH_TYPE|(1L << 43),0, Board::HIGH_TYPE|(1L<<32)|Board::HIGH_TYPE|(1L<<33)|Board::HIGH_TYPE|(1L<<34)|Board::HIGH_TYPE|(1L<<42)|Board::HIGH_TYPE|(1L<<44),
//		Board::HIGH_TYPE|(1L << 44),0, Board::HIGH_TYPE|(1L<<33)|Board::HIGH_TYPE|(1L<<34)|Board::HIGH_TYPE|(1L<<35)|Board::HIGH_TYPE|(1L<<43)|Board::HIGH_TYPE|(1L<<45),
//		Board::HIGH_TYPE|(1L << 45),0, Board::HIGH_TYPE|(1L<<34)|Board::HIGH_TYPE|(1L<<35)|Board::HIGH_TYPE|(1L<<36)|Board::HIGH_TYPE|(1L<<44)|Board::HIGH_TYPE|(1L<<46),
//		Board::HIGH_TYPE|(1L << 46),0, Board::HIGH_TYPE|(1L<<35)|Board::HIGH_TYPE|(1L<<36)|Board::HIGH_TYPE|(1L<<37)|Board::HIGH_TYPE|(1L<<45)|Board::HIGH_TYPE|(1L<<47),
//		Board::HIGH_TYPE|(1L << 47),0, Board::HIGH_TYPE|(1L<<36)|Board::HIGH_TYPE|(1L<<37)|Board::HIGH_TYPE|(1L<<38)|Board::HIGH_TYPE|(1L<<46)|Board::HIGH_TYPE|(1L<<48),
//		Board::HIGH_TYPE|(1L << 48),0, Board::HIGH_TYPE|(1L<<37)|Board::HIGH_TYPE|(1L<<38)|                          Board::HIGH_TYPE|(1L<<47)
//};

void OthelloBoard::load(UL black, UL white) {
	m_black.set(0,0);
	m_white.set(0,0);

	std::bitset<64> b = black;
	std::bitset<64> w = white;
	for (unsigned short k {0}; k < 64; ++k) {
		short a = b[k] ? -1 : w[k] ? 1 : 0;
		if (a) {
			Board& b = a < 0 ? m_black : m_white;
			US K = k % 32;
			short m = K / 8;
			US shift = (m + 1) * 10 + 1 + K % 8;
			UL p = 1L << shift;
			if (k >= 32) {
				p |= Board::HIGH_TYPE;
			}
			b.OR(p);
		    //std::cout << "\n=====================\nk=" << k << ", m=" << m << ", s=" << shift << std::flush;
		    //std::cout << "\n" << *this;
		}
	}
}


/**
 * Compute the so called 'border' which is the number of outside disks
 * @param black
 * @return
 */
US OthelloBoard::border_(bool black) {
	Board& board = this->board(black);
	Board& other = this->board(!black);
	US b = 0;
	board.iterateOnPositions([this, &b, &board, &other](UL pos) {
//		if (board.isOccupied(pos)) {
//			std::bitset<64> pos_(pos);
//			std::cout << "\n$" << pos_ << std::flush;

		board.iterateOnNeighbors(pos, [this, &board, &other, &b, &pos] (UL v, short shift) {
			if (board.isEmpty(v) && other.isEmpty(v)) {
				//print("pos", pos);
				//print("p", p);
				if (board.isOccupied(pos)) {
					b++;
//					std::cout << "," << b << std::flush;
				}
				return false;
			}
			return true;
		});
//		}
		return true;
	});
	return b;
}

/**
 * Compute the so called 'border' which is the number of outside disks
 * @param black
 * @return
 */
US OthelloBoard::border__(bool black) {
	Board& board = this->board(black);
	Board& other = this->board(!black);
	US b = 0;
	const UL *p = &Board::NEIGHBORS_[0];
	int i = 0;
	do {
		UL pos = *p++;//Board::NEIGHBORS_[i];//*p++;
		int n = *p++;//Board::NEIGHBORS_[i + 1];
		if (board.isOccupied(pos)) {
			//std::bitset<64> pos_(pos);
			//std::cout << "\n#" << pos_ << std::flush;
			for (int k = 0; k < n; ++k) {
				UL nei = *p++;//Board::NEIGHBORS_[i + 2 + k];
				if (board.isEmpty(nei) && other.isEmpty(nei)) {
					//print("pos", pos);
					//print("p", p);
//					if (board.isOccupied(pos)) {
						b++;
						//std::cout << "," << b << std::flush;
//					}
					p += n - k - 1;
					break;
				}
			}
		} else {
			p += n;
		}
		i += n + 2;
	} while (i < 548);

//	if (b != border_(black)) {
//		std::cout << board;
//		std::cout << other;
//		b = border(black);
//	}

	return b;
}
/**
 * Compute the so called 'border' which is the number of outside disks
 * @param black
 * @return
 */
US OthelloBoard::border(bool black) {
	Board& board = this->board(black);
	Board& other = this->board(!black);
	const UL low = board.low | other.low;
	const UL high = Board::HIGH_TYPE | board.high | other.high;
	US b = 0;
	const UL *p = &Board::NEIGHBORHOODS[0];
	int i = 0;
	do {
		UL pos = *p++;
		if (pos & board.low) {
			auto low_nei = *p++;
			auto high_nei = *p++;
			if (((low & low_nei) ^ low_nei) | ((high & high_nei) ^ high_nei)) {
				b++;
			}
		} else {
			p += 2;
		}
		i += 3;
	} while (i < 96);
	do {
		UL pos = *p++;
		if (pos & board.high) {
			auto low_nei = *p++;
			auto high_nei = *p++;
			if (((low & low_nei) ^ low_nei) | ((high & high_nei) ^ high_nei)) {
				b++;
			}
		} else {
			p += 2;
		}
		i += 3;
	} while (i < 192);

	return b;
}

/**
 * The mobility is the number of available plays
 * @param black the side to compute
 * @return
 */
US OthelloBoard::mobility(bool black) {
	US m = 0;
//	board(black).iterateOnPositions([this, &m, &black](UL pos) {
//		if (testflip(pos, black)) {
//			m++;
//		}
//		return true;
//	});
	board(black).simpleIterateOnEmptyPositions(board(!black),
		[this, &m, &black](UL pos) {
			if (testflip_noEmptyCheck(pos, black)) {
				m++;
			}
		}
	);
	return m;
}

std::vector<UL> OthelloBoard::mobilityList(bool black) {
	std::vector<UL> v;
	board(black).iterateOnPositions([this, &v, &black](UL pos) {
		if (testflip(pos, black)) {
			v.push_back(pos);
		}
		return true;
	});
	return v;
}

std::vector<UL> OthelloBoard::mobilityListCornersFirst_(bool black) {
	std::vector<UL> v;
	board(black).iterateOnPositionsCornersFirst([this, &v, &black](UL pos) {
		if (testflip(pos, black)) {
			v.push_back(pos);
		}
		return true;
	});
	return v;
}

std::vector<UL> OthelloBoard::mobilityListCornersFirst(bool black) {
	std::vector<UL> v;
	const Board& other = this->board(!black);
	const Board& board = this->board(black);

	board.simpleIterateOnEmptyPositionsCornersFirst(other, [this, &v, &black](UL pos) {
		if (testflip_noEmptyCheck(pos, black)) {
			v.push_back(pos);
		}
	});
	return v;
}


bool OthelloBoard::hasMove(bool black) {
//	return board(black).doWhileOnPositions([this, &black](UL pos) {
//		return testflip(pos, black);
//	});
	return board(black).anyOfEmptyPositions(board(!black),
		[this, &black](UL pos) {
			return canFlip_noEmptyCheck(pos, black);
		}
	);
}


Board& OthelloBoard::blackBoard() {
	return m_black;
}

Board& OthelloBoard::whiteBoard() {
	return m_white;
}


//
//	UL convert(US p) {
//		short j = short(p / 10) - 1, i = p - 10 * (j + 1) - 1;
//		return 1L << (63 - (i * 8 + j));
//	}
//
//	US flip1(US p, bool black, bool test = false) {
//		return flip(convert(p), black);
//	}
//

US OthelloBoard::play(US p, bool black) {
	return flip(toLong(p), black);
}

bool OthelloBoard::canPlay(US p, bool black) {
	return testflip(toLong(p), black);
}

US OthelloBoard::flip(UL p, bool black) {
	Board& otherBoard = this->board(!black);
	Board& board = this->board(black);
	US nf = 0;
	if (isEmpty(p)) {
		//std::cout << std::format("\npos={:d} {:c}", board.convert(p), black ? 'N' : 'B');
		//std::cout << std::flush;
		board.iterateOnNeighbors_(p, [&p, &otherBoard, &board, &nf] (const UL v, short shift) {
			US s1 = 0;
			//std::cout << std::format("\n\tv={:d}, S={:d}", board.convert(v), shift);
			//std::cout << std::flush;
			if (otherBoard.isOccupied(v)) {
				UL cc = v, s = p, t = 0;
				do {
					s1++;
					if ((p & Board::HIGH_BITS) ^ (cc & Board::HIGH_BITS)) {
						t |= cc;
					} else {
						s |= cc;
					}
					cc = board.neighbor(cc, shift);
				} while (otherBoard.isOccupied(cc));

				if (board.isOccupied(cc)) {// && (cc & Board::INTERIOR)) {
					nf += s1;
//						std::cout << std::format(" nf={:d}", nf);
//					std::cout << std::flush;
					board.OR(s);
					board.OR(t);
					otherBoard.AND(~s ^ Board::HIGH_BITS); //exclude 4 high bits
					otherBoard.AND(~t ^ Board::HIGH_BITS);
				}
			}
			//return true;
		});
	}
	return nf;
}

//This method is a hotspot : 50-70% of the time is spent here
US OthelloBoard::testflip(UL p, bool black) {
	Board& otherBoard = this->board(!black);
	Board& board = this->board(black);
	US nf = 0;
	if (isEmpty(p)) {
		//std::cout << std::format("\npos={:d} {:c}", board.convert(p), black ? 'N' : 'B');
		//std::cout << std::flush;
		board.iterateOnNeighbors_(p, [&p, &otherBoard, &board, &nf] (const UL v, short shift) {
			US s1 = 0;
			//std::cout << std::format("\n\tv={:d}, S={:d}", board.convert(v), shift);
			//std::cout << std::flush;
			if (otherBoard.isOccupied(v)) {
				UL cc = v;
				do {
					s1++;
					cc = board.neighbor(cc, shift);
				} while (otherBoard.isOccupied(cc));

				if (board.isOccupied(cc)) {
					nf += s1;
				}
			}
		});
	}
	return nf;
}

//Faster version of testFlip
US OthelloBoard::testflip_noEmptyCheck(UL p, bool black) {//, UL low, UL high) {
	Board& otherBoard = this->board(!black);
	Board& board = this->board(black);
	US nf = 0;
	board.simpleIterateOnOccupiedNeighbors_(p, otherBoard,
		[&p, &otherBoard, &board, &nf] (const UL v, short shift) {
			US s1 = 0;
			UL cc = v;
			bool proceed = true;
			do {
				s1++;
				cc = board.neighbor(cc, shift, otherBoard, proceed);
			} while (proceed);//proceed means otherBoard.isOccupied(cc) = true

			if (board.isOccupied(cc)) {
				nf += s1;
			}
		}
	);
	return nf;
}

US OthelloBoard::canFlip_noEmptyCheck(UL p, bool black) {//, UL low, UL high) {
	Board& otherBoard = this->board(!black);
	Board& board = this->board(black);
	return board.anyOfOccupiedNeighbors_(p, otherBoard,
		[&p, &otherBoard, &board] (const UL v, short shift) {
			US s1 = 0;
			//std::cout << std::format("\n\tv={:d}, S={:d}", board.convert(v), shift);
			//std::cout << std::flush;
			UL cc = v;
			bool proceed = true;
			do {
				s1++;
				cc = board.neighbor(cc, shift, otherBoard, proceed);
			} while (proceed);//proceed means otherBoard.isOccupied(cc) = true

			if (board.isOccupied(cc)) {
				return true;;
			}
			return false;
		}
	);
}

/**
 *
 * @param f
 * @param board
 * @return
 */
std::ostream& operator<<(std::ostream &f, OthelloBoard& board) {
	const US bb = board.border(true);
	const US bw = board.border(false);
	const US mb = board.mobility(true);
	const US mw = board.mobility(false);

	f << "\n    a   b   c   d   e   f   g   h\n";
	f << "    1   2   3   4   5   6   7   8\n";

	f << "  \u250F";//--------------------------\n";
	for (US k {0}; k < 7; ++k) f << "\u2501\u2501\u2501\u252F";
	f << "\u2501\u2501\u2501\u2513\n";

	for (US i {0}; i < 8; ++i) {
		f << (i + 1) << " \u2503";
		for (US j {0}; j < 8; ++j) {
			if (board.m_black.is(j+1, i+1)) {
				f << " \u2B24";
			} else if (board.m_white.is(j+1, i+1)) {
				f << " \u25EF";
			} else {
				f << "  ";
			}
				if (j < 7) {
					f << " \u2502";
				} else {
					f << " ";
				}
		}
		f << "\u2503";
		if (i == 3) {
			f << "    Blancs : " << board.m_white.count_() << " pions (" << bw << "/" << mw << "/" << board.corner(false) << ")" ;
		} else if (i == 4) {
			f << "    Noirs  : " << board.m_black.count_() << " pions (" << bb << "/" << mb << "/" << board.corner(true) << ")";
		}
		f << '\n';

		if (i < 7) {
			f << "  \u2520";
			for (US k {0}; k < 7; ++k) {
				f << "\u2500\u2500\u2500\u253C";
			}
			f << "\u2500\u2500\u2500\u2528\n";
		}
	}

	f << "  \u2517";
	for (US i {0}; i < 7; ++i) f << "\u2501\u2501\u2501\u2537";
	f << "\u2501\u2501\u2501\u251B\n";

	f << std::endl;
	return f;

}


} /* namespace othello */
