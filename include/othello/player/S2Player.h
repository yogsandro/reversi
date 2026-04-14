/*
 * SPlayer.h
 *
 *  Created on: Jun 27, 2024
 *      Author: sandro
 */

#pragma once

#include "othello/eval/SEvaluationStrategy.h"
#include "othello/OthelloBoard.h"
#include "OthelloPlayer.h"
#include "othello/OthelloAlphaBetaTreeBuilder.h"

namespace othello {

class S2Player final : public OthelloPlayer {
public:
	S2Player() = delete;
	S2Player(std::string name, std::string desc, unsigned short maxDepth, SEvaluationStrategy& strategy) :
		OthelloPlayer(name, desc, maxDepth, strategy),
		m_started(false) {
	}
	~S2Player() = default;
	S2Player(const S2Player &other) = default;
	S2Player(S2Player &&other) = default;
	S2Player& operator=(const S2Player &other) = default;
	S2Player& operator=(S2Player &&other) = default;

	virtual UL getMove() override {
		short alpha = std::numeric_limits<short>::min();
		short beta = std::numeric_limits<short>::max();
		auto& board = m_pGameRegistry->board();
		bool blacksTurn = m_pGameRegistry->color(*this);
		short t = m_pGameRegistry->turn();

		if (!m_started) {
			m_ptTree = std::make_shared<OthelloTree>(board, blacksTurn, t);
			m_ptBuilder = std::make_unique<OthelloAlphaBetaTreeBuilder>(*m_ptTree, alpha, beta, m_strategy, m_maxDepth);
			m_ptBuilder->build();
			m_started = true;
		} else {
			UL move = m_pGameRegistry->latestMove();
			std::shared_ptr<Context<Node>> pCtxt;
			std::shared_ptr<Node> pNode;
			//if (move != 0) {
				pNode = m_ptTree->root()->getChild(move);
				if (pNode == nullptr) {
					std::cout << "\nCould not find node" << std::flush;
				}
				m_ptTree->setRoot(pNode);
				pCtxt = m_ptBuilder->rootContext()->getChildContextWithNode(*pNode);
				m_ptBuilder->setRootContext(pCtxt);
//			} else {
//				pNode = m_ptTree->root();
//				pCtxt = m_ptBuilder->rootContext();
//			}
			pCtxt->setAlpha(alpha);
			pCtxt->setBeta(beta);
			//OthelloAlphaBetaTreeBuilder builder(tree, alpha, beta, m_strategy, std::numeric_limits<int>::max(), m_maxDepth);
			//builder.build();
			m_ptBuilder->build();
			//std::cout << *(ptNode.get()) << std::flush;
		}

		auto pCtxt = m_ptBuilder->validatedContext();
		if (pCtxt.get() != nullptr) {
			auto pNode = pCtxt->m_node;

	//		auto move = node.m_board.toStandard(node.m_pos);
	//		std::cout << node.m_board << std::endl;
	//
	//		std::cout << m_ptTree->root()->m_board << std::endl;
	//
			m_ptTree->setRoot(pNode);
			pCtxt = m_ptBuilder->rootContext()->getChildContextWithNode(*pNode);
			m_ptBuilder->setRootContext(pCtxt);
	//		std::cout << m_ptTree->root()->m_board << std::endl;

			//m_ptBuilder->
			return pNode->m_pos;
		}
		Config::VERBOSE = true;
		return 0L;
	}

private:
	bool m_started;

	std::unique_ptr<OthelloAlphaBetaTreeBuilder> m_ptBuilder;
	std::shared_ptr<OthelloTree> m_ptTree;
};

} /* namespace othello */

