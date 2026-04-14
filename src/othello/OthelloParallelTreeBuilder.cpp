/*
 * ParallelTreeBuilder.cpp
 *
 *  Created on: Jun 14, 2024
 *      Author: sandro
 */

#include "othello/OthelloParallelTreeBuilder.h"

#include <vector>

namespace othello {

//Tree<T>& ParallelTreeBuilder::build() {
//	//We start with the root's children
//	std::vector<std::shared_ptr<Node>> startingNodes = retrieveChildrenOfRoot();
//
//	//and push them in the working queue
//	std::for_each(startingNodes.begin(), startingNodes.end(), [this](auto& pNode) {
//		m_queue.push(pNode);
//	});
//
//	std::vector<std::thread> threads;
//
//	try {
//		for(unsigned i {0}; i < m_maxThreads; ++i) {
//			threads.push_back(std::thread(&ParallelTreeBuilder::workerThread, this));
//		}
//
//		std::for_each(threads.begin(), threads.end(), [](std::thread& t) { if (t.joinable()) t.join(); });
//		//std::cout << "\nDone." << std::flush;
//	} catch (...) {
//		std::cout << "Exception caught." << std::flush;
//
//	}
//
//	return m_tree;
//}

//void ParallelTreeBuilder::workerThread() {
//	while(!m_queue.empty()) {
//		//get a node from the queue...
//		auto opt = m_queue.wait_and_pop(250);
//		if (opt.has_value()) {
//			std::shared_ptr<Node> pNode = opt.value();
//			std::vector<std::shared_ptr<Node>> notProcessed;
//			unsigned int nodeCount = 0;
//			buildFrom(pNode, nodeCount, notProcessed);
//			if (notProcessed.size() > 0) {
//				//std::cout << nodeCount << "XXXXX\n";
//				std::for_each(notProcessed.begin(), notProcessed.end(), [this](auto& pNode) {
//					m_queue.push(pNode);
//				});
//			}
//		}
//	}
//}

//std::vector<std::shared_ptr<T>> ParallelTreeBuilder::retrieveChildrenOfRoot() {
//	Node *node = m_tree.root().get();
//	std::vector<std::shared_ptr<Node>> rootsChildren;
//
//	if (node->m_depth + 1 <= m_tree.maxDepth()) {
//		bool black = !node->m_black;
//		OthelloBoard& board = node->m_board;
//
//		board.board(black).iterateOnPositions([this, &rootsChildren, &node, &board, &black](UL pos) {
//			if (board.flip(pos, black, true)) {
//				//nodes.push_back(std::make_shared<Node>(m_turn, node->m_depth + 1, pos, board, black));
//				OthelloBoard b = board;
//				std::shared_ptr<Node> child = std::make_shared<Node>(node->m_turn, node->m_depth + 1, pos, b, black);
//				node->addChild(child);
//				rootsChildren.push_back(child);
//			}
//			return true;
//		});
//	}
//	return rootsChildren;
//}

//void ParallelTreeBuilder::buildFrom(std::shared_ptr<T> pNode, unsigned int& nodeCount, std::vector<std::shared_ptr<T>>& notProcessedNodes) {
//	try {
//	Node *node = pNode.get();
//	node->eval();
//
//	m_strategy.doEval(*node);
//	//std::cout << "\nbuildFrom " << *node << std::flush;
//	if (node->m_depth + 1 <= m_tree.maxDepth()) {
//		bool black = !node->m_black;
//		OthelloBoard& board = node->m_board;
//
//		board.board(black).iterateOnPositions([this, &nodeCount, &notProcessedNodes, &node, &board, &black](UL pos) {
//			if (board.flip(pos, black, true)) {
//				OthelloBoard b = board;
//				//nodes.push_back(std::make_shared<Node>(m_turn, node->m_depth + 1, pos, board, black));
//				std::shared_ptr<Node> child = std::make_shared<Node>(node->m_turn, node->m_depth + 1, pos, b, black);
//				node->addChild(child);
//				count++;
//				if (++nodeCount > m_maxNodes) {
//					//max node is reached, so let's gather the remaining nodes which were not processed.
//					notProcessedNodes.push_back(child);
//				} else {
//					buildFrom(child, nodeCount, notProcessedNodes);
//				}
//			}
//			return true;
//		});
//	}
//	//std::cout << "\nbuildFrom " << *node << " done" << std::flush;
//	} catch (...) {
//		std::cout << "Exception caught." << std::flush;
//
//	}
//}


} /* namespace othello */
