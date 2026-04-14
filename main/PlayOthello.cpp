//============================================================================
// Name        : PlayOthello.cpp
// Author      : 
// Version     :
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <chrono>

#include "othello/GameRegistry.h"
#include "othello/Game.h"
#include "othello/player/S3Player.h"
#include "util/RandomEngineHelper.h"

using namespace std;
using namespace othello;

int main() {
	try {
		std::shared_ptr<GameRegistry> pGameRegistry = std::make_shared<GameRegistry>();
		auto& rnd = rnd::RandomEngineHelper::instance();
		rnd.set_seed(55);

		AlphaBetaEvaluationStrategy abes(*pGameRegistry);
		S3Player s3_("S3\u03B1\u03B2-3", "\u03B1\u03B2 pruning (Depth=3)", 3, abes);
		S3Player s5_("S3\u03B1\u03B2-5", "\u03B1\u03B2 pruning (Depth=5)", 5, abes);

		auto start = std::chrono::high_resolution_clock::now();

		char d = 'n';
		do {
			std::cout << "Voulez-vous les Blancs ou les Noirs [Bb/Nn]? ";
			std::cin >> d;
			std::cout << " ["  << d  << "]\n";
		} while (d != 'b' && d != 'B' && d != 'n' && d != 'N');


		bool choice = d == 'n' || d == 'N';
		pGameRegistry->addPlayer(choice ? s3_ : s5_);
		pGameRegistry->addPlayer(choice ? s5_ : s3_);

		start = std::chrono::high_resolution_clock::now();
		Game game(*pGameRegistry);
		game.init();
		while(!game.hasEnded()) {
			game.play(true);
		}
		game.end();

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
