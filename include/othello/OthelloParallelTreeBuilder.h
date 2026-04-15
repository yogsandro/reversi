/*
 * ParallelTreeBuilder.h
 *
 *  Created on: Jun 14, 2024
 *      Author: sandro
 */

#pragma once

#include <atomic>
#include "OthelloTree.h"
#include "util/ThreadsafeQueue.h"
#include "eval/TreeEvaluationStrategy.h"

namespace othello {

/**
 * This class builds a Tree in parallel.
 */
template<typename T>

class OthelloParallelTreeBuilder final {
public:
	OthelloParallelTreeBuilder() = delete;
	/**
	 *
	 * @param board
	 * @param black
	 * @param turn
	 * @param maxDepth
	 * @param maxNodes
	 * @param n
	 */
	OthelloParallelTreeBuilder(OthelloBoard& board, bool black, US turn, TreeEvaluationStrategy<T>& strategy,
			US maxDepth, int maxNodes,
			unsigned short n = std::thread::hardware_concurrency()) :
		m_tree(OthelloTree(board, black, turn)),
		m_maxNodes(maxNodes),
		m_maxTreeDepth(maxDepth),
		m_maxThreads(n),
		m_strategy (strategy) {
	};
	~OthelloParallelTreeBuilder() = default;
	OthelloParallelTreeBuilder(const OthelloParallelTreeBuilder<T> &other) = default;
	OthelloParallelTreeBuilder(OthelloParallelTreeBuilder<T> &&other) = default;
	OthelloParallelTreeBuilder<T>& operator=(const OthelloParallelTreeBuilder<T> &other) = default;
	OthelloParallelTreeBuilder<T>& operator=(OthelloParallelTreeBuilder<T> &&other) = default;

	/**
	 * Main method of this class: it builds the tree
	 * @return
	 */
	Tree<T>& build() {
		//We start with the root's children
		std::vector<std::shared_ptr<Node>> startingNodes = retrieveChildrenOfRoot();

		//and push them in the working queue
		std::for_each(startingNodes.begin(), startingNodes.end(), [this](auto& pNode) {
			m_queue.push(pNode);
		});

		std::vector<std::thread> threads;

		try {
			for(unsigned i {0}; i < m_maxThreads; ++i) {
				threads.push_back(std::thread(&OthelloParallelTreeBuilder::workerThread, this));
			}

			std::for_each(threads.begin(), threads.end(), [](std::thread& t) { if (t.joinable()) t.join(); });
			//std::cout << "\nDone." << std::flush;
		} catch (...) {
			std::cout << "Exception caught." << std::flush;

		}

		return m_tree;
	}

	std::atomic_long count;

private:
	/**
	 * The result tree
	 */
	OthelloTree m_tree;

	/**
	 * The max number of nodes processed by each thread
	 */
	unsigned int m_maxNodes;

	/**
	 * Max evaluation tree depth
	 */
	unsigned short m_maxTreeDepth;

	/**
	 * Max number of thread (by default the number of available threads of the system)
	 */
	unsigned short m_maxThreads;

	/**
	 * A threadsafe queue
	 */
	ts_queue<T> m_queue;


	TreeEvaluationStrategy<T>& m_strategy;

	/**
	 * Each thread starts from a node and builds a predefined number of child nodes (nodeCount). The remaining
	 * (notProcessedNodes) are put in the (threadsafe) queue and will be precessed by other threads.
	 * @param node
	 * @param nodeCount
	 * @param notProcessedNodes
	 */
	void buildFrom(std::shared_ptr<T> pNode, unsigned int& nodeCount, std::vector<std::shared_ptr<T>>& notProcessedNodes) {
		try {
		T *node = pNode.get();
		node->eval();

		m_strategy.doEval(*node);
		//std::cout << "\nbuildFrom " << *node << std::flush;
		if (node->m_depth + 1 <= m_maxTreeDepth) {
			bool black = !node->m_black;
			OthelloBoard& board = node->m_board;
//			board.board(black).iterateOnPositions([this, &nodeCount, &notProcessedNodes, &node, &board, &black](UL pos) {
			board.board(black).simpleIterateOnEmptyPositions(board.board(!black),
				[this, &nodeCount, &notProcessedNodes, &node, &board, &black](UL pos) {
//					if (board.testflip(pos, black)) {
					if (board.testflip_noEmptyCheck(pos, black)) {
						OthelloBoard b = board;
						//nodes.push_back(std::make_shared<Node>(m_turn, node->m_depth + 1, pos, board, black));
						std::shared_ptr<T> child = std::make_shared<T>(node->m_turn, node->m_depth + 1, pos, b, black);
						node->addChild(child);
						count++;
						if (++nodeCount > m_maxNodes) {
							//max node is reached, so let's gather the remaining nodes which were not processed.
							notProcessedNodes.push_back(child);
						} else {
							buildFrom(child, nodeCount, notProcessedNodes);
						}
					}
//					return true;
				}
			);
		}
		//std::cout << "\nbuildFrom " << *node << " done" << std::flush;
		} catch (...) {
			std::cout << "Exception caught." << std::flush;

		}
	}

	/**
	 *
	 * @return
	 */
	std::vector<std::shared_ptr<T>> retrieveChildrenOfRoot() {
		T *node = m_tree.root().get();
		std::vector<std::shared_ptr<Node>> rootsChildren;

		if (node->m_depth + 1 <= m_maxTreeDepth) {
			bool black = !node->m_black;
			OthelloBoard& board = node->m_board;

			board.board(black).iterateOnPositions([this, &rootsChildren, &node, &board, &black](UL pos) {
				if (board.testflip(pos, black)) {
					//nodes.push_back(std::make_shared<Node>(m_turn, node->m_depth + 1, pos, board, black));
					OthelloBoard b = board;
					std::shared_ptr<T> child = std::make_shared<T>(node->m_turn, node->m_depth + 1, pos, b, black);
					node->addChild(child);
					rootsChildren.push_back(child);
				}
				return true;
			});
		}
		return rootsChildren;
	}

	/**
	 * This function is started by each thread
	 */
	void workerThread() {
		while(!m_queue.empty()) {
			//get a node from the queue...
			auto opt = m_queue.wait_and_pop(250);
			if (opt.has_value()) {
				std::shared_ptr<Node> pNode = opt.value();
				std::vector<std::shared_ptr<Node>> notProcessed;
				unsigned int nodeCount = 0;
				buildFrom(pNode, nodeCount, notProcessed);
				if (notProcessed.size() > 0) {
					//std::cout << nodeCount << "XXXXX\n";
					std::for_each(notProcessed.begin(), notProcessed.end(), [this](auto& pNode) {
						m_queue.push(pNode);
					});
				}
			}
		}
	}


};



} /* namespace othello */


