/*
 * S3Player.h
 *
 *  Created on: Jun 12, 2025
 *      Author: sandro
 */

#pragma once

#include <cassert>
#include "othello/OthelloBoard.h"
#include "AlphaBetaOthelloPlayer.h"
#include "othello/AlphaBetaOthelloTree.h"

namespace othello {

class S3Player final : public AlphaBetaOthelloPlayer {
public:
	S3Player() = delete;
	S3Player(std::string name, std::string desc, unsigned short maxDepth, AlphaBetaEvaluationStrategy& strategy) :
		AlphaBetaOthelloPlayer(name, desc, maxDepth, strategy),
		m_started(false) {
	}
	~S3Player() = default;
	S3Player(const S3Player &other) = default;
	S3Player(S3Player &&other) = default;
	S3Player& operator=(const S3Player &other) = default;
	S3Player& operator=(S3Player &&other) = default;

	virtual void init() override {
		m_started = false;
	}

	virtual void end() override {
		m_ptTree->free();
		m_ptBuilder->free();
		m_ptTree = nullptr;
		m_ptBuilder = nullptr;
	}

	virtual UL getMove() override {
		short alpha = std::numeric_limits<short>::min();
		short beta = std::numeric_limits<short>::max();
		auto& board = m_pGameRegistry->board();
		bool blacksTurn = m_pGameRegistry->color(*this);

		if (!m_started) {
			m_ptTree = std::make_shared<AlphaBetaOthelloTree>(board, blacksTurn);
			m_ptBuilder = std::make_unique<AlphaBetaOthelloTreeBuilder>(*m_ptTree, alpha, beta, m_strategy, m_maxDepth);
			m_ptBuilder->build(board);
			m_started = true;
		} else {
			//gets the latest move in UL type pos
			UL move = OthelloBoard::toLong(m_pGameRegistry->latestMove());

			//the node containing the latest move will be the new root
			std::shared_ptr<AlphaBetaNode> pNode;
			pNode = m_ptTree->root()->getChild(move);

			assert(pNode != nullptr && "Could not find node");

			//breaks the cyclic dependency between nodes to release memory :
			//(node -> parent node -> children (node amongst children)
			pNode->setParent(nullptr);
			std::cout << "\nM" << std::flush;
			long count = m_ptTree->root()->freeUntil(pNode);
			std::cout << "M" << std::flush;
			m_ptBuilder->m_totalNodeCount -= count;
			m_ptTree->setRoot(pNode);
			m_ptBuilder->setRootContext(pNode);

			pNode->setAlpha(alpha);
			pNode->setBeta(beta);
			m_ptBuilder->build(board);
			//std::cout << *(ptNode.get()) << std::flush;
		}
		//std::cout << "\nA" << std::flush;
		auto pNode = m_ptBuilder->validated();
		//std::cout << "\nA" << std::flush;
		if (pNode.get() != nullptr) {
			//breaks the cyclic dependency between nodes to release memory
			pNode->setParent(nullptr);
			//std::cout << "\nFU..." << std::flush;
			long count = m_ptTree->root()->freeUntil(pNode);
			//std::cout << "done." << std::flush;
			m_ptBuilder->m_totalNodeCount -= count;
			//std::cout << "\nAAA" << std::flush;
			m_ptTree->setRoot(pNode);
			//std::cout << "\nAAAA" << std::flush;
			m_ptBuilder->setRootContext(pNode);
			//std::cout << "\nAAAAA" << std::flush;
			return pNode->m_upos ? OthelloBoard::toLong(pNode->m_upos) : 0L;
		}
		return 0L;
	}

private:
	bool m_started;

	std::unique_ptr<AlphaBetaOthelloTreeBuilder> m_ptBuilder;
	std::shared_ptr<AlphaBetaOthelloTree> m_ptTree;
};

} /* namespace othello */

