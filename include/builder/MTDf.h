/*
 * MTDf.h
 *
 *  Created on: Jul 13, 2024
 *      Author: sandro
 */

#pragma once

#include "ParallelAlphaBetaTreeBuilder.h"

namespace othello {

template<typename T>
class MTDf final {
public:
	MTDf(AlphaBetaTreeBuilder<T>& builder) :
		m_builder(builder) {
	}
	MTDf() = delete;
	~MTDf() = default;
	MTDf(const MTDf &other) = default;
	MTDf(MTDf &&other) = default;
	MTDf& operator=(const MTDf &other) = default;
	MTDf& operator=(MTDf &&other) = default;

	short build(short firstGuess, short depth) {
		short g = firstGuess;

		short beta;

		short l = std::numeric_limits<short>::min();
		short u = std::numeric_limits<short>::max();

		short iter = 0;

		do {
			//if (depth == 9 && iter >= 2) Config::VERBOSE = true;

			if (Config::VERBOSE) std::cout << "\n****** ID" << iter << ", g=" << g << ", [" << l << ", " << u << "]" << std::flush;

			if (g == l) {
				beta = g + 1;
			} else {
				beta  = g;
			}

			if (Config::VERBOSE) std::cout << " => \u03B2=" << beta << ", (\u03B1=" << (beta -1) << ", \u03B2=" << beta << ") ******" << std::endl;

			m_builder.setMaxTreeDepth(depth);

			m_builder.setAlpha(beta - 1);
			m_builder.setBeta(beta);

			m_builder.setSortFunction([&iter, &depth](std::vector<std::shared_ptr<T>>& nodes) {
				if (iter > 0) return;
				const unsigned S = nodes.size();
				if (S <= 1) {
					return;
				}

				unsigned short d = nodes[0]->m_depth;
				if (d <= depth - 1) {
	//#ifdef DEBUG
//					if (Config::VERBOSE)  {
//						std::cout << "\nSORTING depth="<< d << std::flush;
//						std::cout << "\nSorting nodes (";
//						for (const auto &pNode : nodes) {
//							std::cout << *(pNode.get()) << ", ";
//						}
//						std::cout << ")"; std::cout << std::flush;
//					}
	//#endif

					std::sort(nodes.begin(), nodes.end(), [&d](std::shared_ptr<T>& pi, std::shared_ptr<T>& pj) {
						return 	d % 2 == 1 ? pi.get()->m_value > pj.get()->m_value : pi.get()->m_value < pj.get()->m_value;
					});

	//#ifdef DEBUG
//					if (Config::VERBOSE) {
//						std::cout << "\nSorting nodes (";
//						for (const auto &pNode : nodes) {
//							std::cout << *(pNode.get()) << ", ";
//						}
//						std::cout << ")"; std::cout << std::flush;
//						std::cout << "\nEND SORTING depth="<< d << std::flush;
//					}
	//#endif
				}
			});

			if (iter > 0) {
				std::vector<std::shared_ptr<Context<T>>>& startingContext = m_builder.rootContext()->m_children;
				m_builder.rootContext()->setAlpha(beta -1);
				m_builder.rootContext()->setBeta(beta);

				std::sort(startingContext.begin(), startingContext.end(), [](std::shared_ptr<Context<T>>& pi, std::shared_ptr<Context<T>>& pj) {
					return 	pi->m_value > pj->m_value;
				});

				m_builder.rootContext()->m_value = std::numeric_limits<short>::min();
//				m_builder.rootContext()->clear(depth - 1, beta - 1);
//				m_builder.rootContext()->clear(1, beta - 1);

				//init root context
				m_builder.rootContext()->open();
//				m_builder.rootContext()->clear(depth);
//				m_builder.rootContext()->m_closed = false;
				m_builder.rootContext()->m_count = m_builder.rootContext()->m_card;
				m_builder.startsFrom(startingContext);


			} else {
				m_builder.build();
			}
			g = m_builder.value();

			if (g < beta) {
				u = g;
			} else {
				l = g;
			}
			if (Config::VERBOSE) std::cout << "\n****** BUILT => g=" << g << ", [" << l << ", " << u << "]" << ", \u03B1=" << m_builder.alpha() << ", \u03B2=" << m_builder.beta() << std::endl;


			//showContext_(builder);

			//builder.setTree(copy);
			iter++;

		} while(l < u);

//		m_builder.clearCache();

//		m_builder.rootContext().get()->clear(depth - 1, beta - 1);

		return g;
	}

private:
	AlphaBetaTreeBuilder<T>& m_builder;
};

} /* namespace othello */

