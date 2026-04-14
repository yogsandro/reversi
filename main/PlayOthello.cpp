//============================================================================
// Name        : PlayOthello.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <chrono>
#include <map>
#include <random>
#include <concepts>
#include <sstream>
#include <cassert>
#include <chrono>
#include <cmath>
#include <list>
#include <array>
#include <deque>


#include "othello/GameRegistry.h"
#include "othello/Game.h"
#include "othello/player/MTDf3Player.h"
#include "othello/OthelloBoard.h"
#include "othello/OthelloTree.h"
#include "othello/eval/SEvaluationStrategy.h"
#include "othello//eval/GEvaluationStrategy.h"
#include "othello/SimpleOthelloTreeBuilder.h"
#include "builder/IterativeDeepening.h"
#include "builder/MTDf.h"
#include "othello/player/GPlayer.h"
#include "othello/player/HumanPlayer.h"
#include "othello/player/JSPlayer.h"
#include "othello/player/SPlayer.h"
#include "othello/player/S2Player.h"
#include "othello/player/MTDfPlayer.h"
#include "othello/player/MTDf3Player.h"
#include "othello/player/S3Player.h"
#include "othello/OthelloAlphaBetaTreeBuilder.h"
#include "util/RandomEngineHelper.h"


#include "test/TestTreeBuilder.h"

using namespace std;
using namespace othello;

int main() {
	try {
		std::shared_ptr<GameRegistry> pGameRegistry = std::make_shared<GameRegistry>();
		auto& rnd = rnd::RandomEngineHelper::instance();
		rnd.set_seed(55);

		HumanPlayer human("Sandro");
		HumanPlayer b("Sandro2");
		HumanPlayer pc("PC");
		JSPlayer js("J&S", "original J&S algo");
		JSPlayer js2("J&S2", "original J&S algo");

		GEvaluationStrategy sD1_510_ = GEvaluationStrategy(1, 5, 10);
		GEvaluationStrategy sD2_510_ = GEvaluationStrategy(2, 5, 10);
		GEvaluationStrategy sD3_510_ = GEvaluationStrategy(3, 5, 10);
		GEvaluationStrategy sD3_105_ = GEvaluationStrategy(3, 1, 35);
		GEvaluationStrategy sD3_510a_ = GEvaluationStrategy(3, 5, 10);
		GEvaluationStrategy sD5_510_ = GEvaluationStrategy(5, 5, 10);
		GEvaluationStrategy sD6_510_ = GEvaluationStrategy(6, 5, 10);
		GEvaluationStrategy sD7_510_ = GEvaluationStrategy(7, 5, 10);
		SEvaluationStrategy es = SEvaluationStrategy();
		AlphaBetaEvaluationStrategy abes(*pGameRegistry);
		GPlayer giuseppe1("GD1", "Algo de Giuseppe (Depth=1)", sD1_510_);
		GPlayer giuseppe2("GD2", "Algo de Giuseppe (Depth=2)", sD2_510_);
		GPlayer giuseppe3("GD3", "Algo de Giuseppe (Depth=3)", sD3_510_);
		GPlayer giuseppe3_("GD3", "Algo de Giuseppe (Depth=3)", sD3_510a_);
		GPlayer giuseppe3a("GD3a", "Algo de Giuseppe (Depth=3)", sD3_105_);
		GPlayer giuseppe5("GD5", "Algo de Giuseppe (Depth=5)", sD5_510_);
		GPlayer giuseppe6("GD6", "Algo de Giuseppe (Depth=6)", sD6_510_);
		GPlayer giuseppe7("GD7", "Algo de Giuseppe (Depth=7)", sD7_510_);
		SPlayer sandro9("S\u03B1\u03B2-9", "\u03B1\u03B2 pruning (Depth=9)", 9, es);
		SPlayer sandro8("S\u03B1\u03B2-8", "\u03B1\u03B2 pruning (Depth=8)", 8, es);
		SPlayer sandro7("S\u03B1\u03B2-7", "\u03B1\u03B2 pruning (Depth=7)", 7, es);
		SPlayer sandro5("S\u03B1\u03B2-5", "\u03B1\u03B2 pruning (Depth=5)", 5, es);
		SPlayer sandro3("S\u03B1\u03B2-3", "\u03B1\u03B2 pruning (Depth=3)", 3, es);

		S2Player s3("S2\u03B1\u03B2-5", "\u03B1\u03B2 pruning (Depth=5)", 5, es);
		S2Player s7("S2\u03B1\u03B2-7", "\u03B1\u03B2 pruning (Depth=7)", 5, es);
		S2Player s9("S2\u03B1\u03B2-9", "\u03B1\u03B2 pruning (Depth=9)", 9, es);

		S3Player s3_("S3\u03B1\u03B2-3", "\u03B1\u03B2 pruning (Depth=3)", 3, abes);
		S3Player s5_("S3\u03B1\u03B2-5", "\u03B1\u03B2 pruning (Depth=5)", 5, abes);
		S3Player s7_("S3\u03B1\u03B2-7", "\u03B1\u03B2 pruning (Depth=7)", 7, abes);
		S3Player s9_("S3\u03B1\u03B2-9", "\u03B1\u03B2 pruning (Depth=9)", 9, abes);


		MTDfPlayer mtdf7("MTD(f)", "MTD(f) iterative deepening, depth=7", 5, es);
		MTDf3Player mtdf3_5("MTD(f)-35", "MTD(f) iterative deepening, depth=5", 5, abes);
		MTDf3Player mtdf3_7("MTD(f)-37", "MTD(f) iterative deepening, depth=7", 7, abes);
		MTDf3Player mtdf3_9("MTD(f)-39", "MTD(f) iterative deepening, depth=9", 9, abes);
		MTDfPlayer mtdf8("MTD(f)", "MTD(f) iterative deepening, depth=8", 8, es);
		MTDfPlayer mtdf9("MTD(f)", "MTD(f) iterative deepening, depth=9", 9, es);
		MTDfPlayer mtdf10("MTD(f)", "MTD(f) iterative deepening, depth=10", 10, es);

		auto start = std::chrono::high_resolution_clock::now();

		//Config::VERBOSE = true;
		if (true) {
			char d = 'n';
			//if (false)
			do {
				std::cout << "Voulez-vous les Blancs ou les Noirs [Bb/Nn]? ";
				std::cin >> d;
				std::cout << " ["  << d  << "]\n";
			} while (d != 'b' && d != 'B' && d != 'n' && d != 'N');


			bool choice = d == 'n' || d == 'N';

			if (choice) {
				pGameRegistry->addPlayer(s3_);//mtdf7);
				pGameRegistry->addPlayer(s5_);//mtdf7);
//				game->addPlayer(mtdf3_5);
//				game->addPlayer(sandro5);
//				game->addPlayer(sandro7);
			} else {
				//game->addPlayer(sandro9);
//				game->addPlayer(mtdf3_5);
				pGameRegistry->addPlayer(s5_);//mtdf7);
				pGameRegistry->addPlayer(s3_);//mtdf7);
//				game->addPlayer(sandro7);
//				game->addPlayer(sandro5);
			}

			start = std::chrono::high_resolution_clock::now();
			Game game(*pGameRegistry);
			game.init();
			while(!game.hasEnded()) {
				game.play(true);
			}
			game.end();

			//while(game.haRegistry.)
		} else {
//			bool silent = true;
//			js.setSilent(silent);
//			js2.setSilent(silent);
//			giuseppe1.setSilent(silent);
//			giuseppe2.setSilent(silent);
//			giuseppe3.setSilent(silent);
//			giuseppe3_.setSilent(silent);
//			giuseppe3a.setSilent(silent);
//			giuseppe5.setSilent(silent);
//			giuseppe6.setSilent(silent);
//			sandro7.setSilent(silent);
//			sandro5.setSilent(silent);
//			sandro3.setSilent(silent);
//			s3.setSilent(silent);
//			s3_.setSilent(silent);
//			s5_.setSilent(silent);
//			s7_.setSilent(silent);
//			mtdf7.setSilent(silent);
//			const int N = 100;
//			int sw[] = {0,0,0};
//			start = std::chrono::high_resolution_clock::now();
//			for(int i {0}; i < N; ++i) {
//				std::shared_ptr<GameRegistry> game = std::make_shared<GameRegistry>();
////				game->addPlayer(sandro5);
////				game->addPlayer(sandro7);
//				game->addPlayer(s5_);
//				game->addPlayer(s3_);
//				short w = game->start(false);
//				sw[w+1]++;
//				std::cout << "(" << sw[0] << "," << sw[1] << "," << sw[2] << ")\n" << std::flush;
//			}
//
//				sw[0] = sw[1] = sw [2] = 0;
////			bool render = false;
//			for(int i {0}; i < N; ++i) {
////				s3_.setSilent(i != 11);
////				s5_.setSilent(i != 11);
////				render = i == 11;
//				std::shared_ptr<GameRegistry> game = std::make_shared<GameRegistry>();
//
////				game->addPlayer(sandro7);
//				game->addPlayer(s3_);
////				game->addPlayer(sandro5);
//				game->addPlayer(s5_);
//				short w = game->start(false);
//				sw[w+1]++;
//				std::cout << "(" << sw[0] << "," << sw[1] << "," << sw[2] << ")\n" << std::flush;
//			}
		}

		auto end = std::chrono::high_resolution_clock::now();
		auto m = std::chrono::duration_cast<std::chrono::minutes> (end - start).count();
		auto s = std::chrono::duration_cast<std::chrono::seconds> (end - start).count();
		auto ms = std::chrono::duration_cast<std::chrono::milliseconds> (end - start).count();

		std::cout << "Temps de la partie: " << m << " minutes, " << long(s - 60 * m) << " secondes et " << (long(ms) - 1000 * s) << " millisecondes." << std::endl;
	}catch (std::runtime_error& e) {
		std::cout << "\n" << e.what() << std::endl;
	}catch (std::string& s) {
		std::cout << "\n" << s << std::endl;
	}catch (...) {
		std::cout << "\nUnknown exception caught..." << std::endl;
	}
}
