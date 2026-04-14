/*
 * OthelloALPHABETATreeBuilder.h
 *
 *  Created on: Jun 23, 2024
 *      Author: sandro
 */

#pragma once

#include <atomic>
#include <mutex>
#include <vector>
#include <memory>
#include <execution>
#include <sstream>

#include "OthelloTree.h"
#include "builder/ParallelAlphaBetaTreeBuilder.h"
#include "util/ThreadsafeQueue.h"
#include "eval/EvaluationStrategy.h"

namespace othello {

class OthelloAlphaBetaTreeBuilder final : public AlphaBetaTreeBuilder<Node> {
public:
	OthelloAlphaBetaTreeBuilder(Tree<Node>& tree, short alpha, short beta,
		EvaluationStrategy<Node>& strategy,
		int maxTreeDepth) :
		AlphaBetaTreeBuilder(tree, alpha, beta, maxTreeDepth),
		m_strategy (strategy) {
	};
	OthelloAlphaBetaTreeBuilder() = delete;
	~OthelloAlphaBetaTreeBuilder() = default;
	OthelloAlphaBetaTreeBuilder(const OthelloAlphaBetaTreeBuilder &other) = default;
	OthelloAlphaBetaTreeBuilder(OthelloAlphaBetaTreeBuilder &&other) = default;
	OthelloAlphaBetaTreeBuilder& operator=(const OthelloAlphaBetaTreeBuilder &other) = default;
	OthelloAlphaBetaTreeBuilder& operator=(OthelloAlphaBetaTreeBuilder &&other) = default;

	/**
	 *
	 * @return
	 */
	virtual std::vector<std::shared_ptr<Context<Node>>> retrieveInitialContext() override {
		auto pNode = m_tree.root();


		std::vector<std::shared_ptr<Context<Node>>> contexts;

		if (!pNode->hasChild()) {
			if (pNode->m_depth + 1 <= this->m_maxTreeDepth) {
				//std::vector<std::shared_ptr<Node>> pNodes = getNodesFrom(node);

				std::vector<std::shared_ptr<Node>>& pNodes = pNode->m_children;
				if (!pNode->hasChild()) {
					getNodesFrom(*pNode);
				} else {
					sortNodes(pNodes);
				}

				m_rootContext = std::make_shared<Context<Node>>(pNode, m_alpha, m_beta, pNodes.size());

				for (std::shared_ptr<Node>& pChild : pNodes) {
					std::shared_ptr<Context<Node>> pChildCtxt = std::make_shared<Context<Node>>(pChild, m_rootContext);
					contexts.push_back(pChildCtxt);
					m_rootContext->m_children.push_back(pChildCtxt);
				}
			}
		} else {
			m_rootContext->init();
			for (auto pChildCtxt : m_rootContext->m_children) {
				contexts.push_back(pChildCtxt);
			}
//			auto leafs = m_rootContext->leafs();
//
//#ifdef XXX
//			std::mutex mutex;
//			std::for_each(std::execution::par, leafs.begin(), leafs.end(), [this, &contexts, &mutex](auto& leaf) {
//#ifdef __DEBUG__
//				if (Config::VERBOSE) {
//					std::cout << "\n" << "d=" << leaf->m_node->m_depth <<" - " << pNode->m_depth << " = " << leaf->m_node->m_depth - pNode->m_depth << std::flush;
//				}
//#endif
//				if (leaf->m_node->m_depth + 1 <= this->m_maxTreeDepth) {
//					//std::vector<std::shared_ptr<Node>> pNodes = getNodesFrom(node);
//
//					std::vector<std::shared_ptr<Node>>& pNodes = leaf->m_node->m_children;
//					if (!leaf->m_node->hasChild()) {
//						getNodesFrom(*leaf->m_node);
//					} else {
//						sortNodes(pNodes);
//					}
//
////					m_rootContext = std::make_shared<Context<Node>>(node, m_alpha, m_beta, pNodes.size());
//
//					for (std::shared_ptr<Node>& pChild : pNodes) {
//						std::shared_ptr<Context<Node>> pChildCtxt = std::make_shared<Context<Node>>(pChild, leaf);
//						{
//							std::lock_guard lock(mutex);
//							contexts.push_back(pChildCtxt);
//						}
//						leaf->m_children.push_back(pChildCtxt);
//					}
//
//				}
//			});
//
//#endif
//			for(auto leaf : leafs) {
//#ifdef __DEBUG__
//				if (Config::VERBOSE) {
//					std::cout << "\n" << "d=" << leaf->m_node->m_depth <<" - " << pNode->m_depth << " = " << leaf->m_node->m_depth - pNode->m_depth << std::flush;
//				}
//#endif
//				if (leaf->m_node->m_depth + 1 <= this->m_maxTreeDepth) {
//					//std::vector<std::shared_ptr<Node>> pNodes = getNodesFrom(node);
//
//					std::vector<std::shared_ptr<Node>>& pNodes = leaf->m_node->m_children;
//					if (!leaf->m_node->hasChild()) {
//						getNodesFrom(*leaf->m_node);
//					} else {
//						sortNodes(pNodes);
//					}
//
////					m_rootContext = std::make_shared<Context<Node>>(node, m_alpha, m_beta, pNodes.size());
//
//					for (std::shared_ptr<Node>& pChild : pNodes) {
//						std::shared_ptr<Context<Node>> pChildCtxt = std::make_shared<Context<Node>>(pChild, leaf);
//						contexts.push_back(pChildCtxt);
//						leaf->m_children.push_back(pChildCtxt);
//					}
//
//				}
//			}
		}

		return contexts;
	}
	/**
	 *
	 * @return
	 */
//	virtual std::vector<std::shared_ptr<Context<Node>>> retrieveInitialContext() override {
//		Node& node = *m_tree.root();
//
//		auto leafs = m_tree.leafs();
//
//		std::vector<std::shared_ptr<Context<Node>>> contexts;
//
//		if (node.m_depth + 1 <= this->m_maxTreeDepth) {
//			//std::vector<std::shared_ptr<Node>> pNodes = getNodesFrom(node);
//
//			std::vector<std::shared_ptr<Node>>& pNodes = node.m_children;
//			if (!node.hasChild()) {
//				getNodesFrom(node);
//			} else {
//				sortNodes(pNodes);
//			}
//
//			m_rootContext = std::make_shared<Context<Node>>(node, m_alpha, m_beta, pNodes.size());
//
//			for (std::shared_ptr<Node>& pChild : pNodes) {
//				std::shared_ptr<Context<Node>> pChildCtxt = std::make_shared<Context<Node>>(*(pChild.get()), m_rootContext);
//				contexts.push_back(pChildCtxt);
//				m_rootContext->m_children.push_back(pChildCtxt);
//			}
//
//		}
//		return contexts;
//	}

	/**
	 * @param node
	 */
	virtual void buildFrom_(std::shared_ptr<Context<Node>>& pCtxt) {//override {
//		try {
		//Context<Node>& ctxt = *pCtxt;
		auto& pNode = pCtxt->m_node;
//		ctxt.update();
#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::cout << "\nExploring node " << *pNode << std::flush;
		}
#endif
		//bool isLeaf = node.m_depth == this->m_maxTreeDepth;

		//bool retrieved = m_useCache ? retrieve(ctxt) : false;

		auto& parentCtxt = pCtxt->m_parent;
		if (parentCtxt != nullptr && parentCtxt->m_closed) {
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << "\nParent context is closed: " << *parentCtxt << std::flush;
			}
#endif
			return;
		}

		if (pCtxt->m_closed) {
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << "\nNode '" << *pNode << " is closed" << std::flush;
			}
#endif
			return;
		}

//		if (retrieved) {
//			throw "RRRRR";
//		}
//		if (!retrieved) {
			pNode->eval();
//		}

		bool proceed = pNode->m_depth + 1 <= this->m_maxTreeDepth;

		if (proceed) {

			//ctxt.update();
			std::vector<std::shared_ptr<Node>>& pNodes = pNode->m_children;
			unsigned short s = pNodes.size();

			if (s == 0) {
				getNodesFrom(*pNode);
				s = pNodes.size();
			} else {
				sortNodes(pNodes);
			}

			//still some more move available ?
			proceed = s > 0;

			if (proceed) {
				if (pCtxt->m_card == -1) {
					pCtxt->setCard(s);
				}
				for (std::shared_ptr<Node>& pChild : pNodes) {
					//Node& child = *pChild;

					std::shared_ptr<Context<Node>> pChildCtxt = pCtxt->getChildContextWithNode(*pChild);

//					for (auto& p : pCtxt->m_children) {
//						if (p->m_node->m_pos == pChild->m_pos) {
//							pChildCtxt = p;
//							break;
//						}
//					}

					if (pChildCtxt == nullptr) {
#ifdef __DEBUG__
						if (Config::VERBOSE) {
							std::cout << "\nCreating context for " << pChild << std::flush;
						}
#endif
						pChildCtxt = std::make_shared<Context<Node>>(pChild, pCtxt);
						pCtxt->addChild(pChildCtxt);//m_children.push_back(pChildCtxt);
					}
					//The following cases are valid when we use a previously root context
					else { //if (pNode->m_depth + 1 < this->m_maxTreeDepth) {
#ifdef __DEBUG__
						if (Config::VERBOSE) {
							std::cout << "\nInitializing context: " << *pChildCtxt; std::cout << std::flush;
						}
#endif
						pChildCtxt->init(pCtxt);
#ifdef __DEBUG__
						if (Config::VERBOSE) {
							std::cout << "\nInitialized context: " << *pChildCtxt; std::cout << std::flush;
						}
#endif
					}
//					else {
//#ifdef __DEBUG__
//						if (Config::VERBOSE) {
//							std::cout << "\nExisting & proceeded context: " << *pChildCtxt; std::cout << std::flush;
//						}
//#endif
//						std::cout << "\nExisting & proceeded context: " << *pChildCtxt; std::cout << std::flush;
//						continue;
//					}

					m_totalNodeCount++;
					buildFrom(pChildCtxt);
					if (pCtxt->m_closed) {
#ifdef __DEBUG__
							if (Config::VERBOSE) {
								std::cout << " closing " << *pNode;
								std::cout << std::flush;
							}
#endif
						break;
					}
				}
			}
		}

		if (!proceed) {
			//We've reached a tree leaf
			//if (!retrieved) {
				m_strategy.eval(*pNode);
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\nLEAF - " << *pNode << std::flush;
				}
#endif
				pCtxt->m_value = (2 * (pNode->m_depth%2 - 1) + 1) * pNode->m_value;
			//}
			//ctxt.m_value = node.m_value;
			//if parent context of ctxt is closed before count down, updating it (by passing here from
			//children) may decrease the count down of its parent (if it is not closed) and finally may
			//close it (parent of parent of ctxt) avoiding other children to update it
			//if (!ctxt.m_parent.get()->m_closed) {
			updateAll(*pCtxt);//.m_parent.get()->updateAll(ctxt);
			//}
		}
		//std::cout << "\nbuildFrom " << *node << " done" << std::flush;
//		} catch (...) {
//			std::cout << "Exception caught." << std::flush;
//
//		}
	}
	virtual void buildFrom(std::shared_ptr<Context<Node>>& pCtxt) override {
		auto& pNode = pCtxt->m_node;
		auto& parentCtxt = pCtxt->m_parent;
		if (parentCtxt != nullptr && parentCtxt->m_closed) {
			return;
		}

		if (pCtxt->m_closed) {
			return;
		}

		pNode->eval();

		bool proceed = pNode->m_depth + 1 <= this->m_maxTreeDepth;

		if (proceed) {

			//ctxt.update();
			std::vector<std::shared_ptr<Node>>& pNodes = pNode->m_children;
			unsigned short s = pNodes.size();

			if (s == 0) {
				getNodesFrom(*pNode);
				s = pNodes.size();
			} else {
				sortNodes(pNodes);
			}

			//still some more move available ?
			proceed = s > 0;

			if (proceed) {
				if (pCtxt->m_card == -1) {
					pCtxt->setCard(s);
				}
				for (std::shared_ptr<Node>& pChild : pNodes) {
					std::shared_ptr<Context<Node>> pChildCtxt = pCtxt->getChildContextWithNode(*pChild);

//					for (auto& p : pCtxt->m_children) {
//						if (p->m_node->m_pos == pChild->m_pos) {
//							pChildCtxt = p;
//							break;
//						}
//					}

					if (pChildCtxt == nullptr) {
						pChildCtxt = std::make_shared<Context<Node>>(pChild, pCtxt);
						pCtxt->addChild(pChildCtxt);//m_children.push_back(pChildCtxt);
					}
					//The following cases are valid when we use a previously root context
					else { //if (pNode->m_depth + 1 < this->m_maxTreeDepth) {
						pChildCtxt->init(pCtxt);
					}

					m_totalNodeCount++;
					buildFrom(pChildCtxt);
					if (pCtxt->m_closed) {
						break;
					}
				}
			}
		}

		if (!proceed) {
			//We've reached a tree leaf
			m_strategy.eval(*pNode);
			pCtxt->m_value = (2 * (pNode->m_depth%2 - 1) + 1) * pNode->m_value;
			updateAll(*pCtxt);//.m_parent.get()->updateAll(ctxt);
		}
	}

private:

	EvaluationStrategy<Node>& m_strategy;

	/**
	 *
	 * @param board
	 * @param black
	 * @return
	 */
	std::vector<std::shared_ptr<Node>>& getNodesFrom(Node& node) {
		OthelloBoard& board = node.m_board;
		bool black = !node.m_black;
//		const std::vector<UL> list = board.mobilityList(black);
		const std::vector<UL> list = board.mobilityListCornersFirst(black);
		const unsigned short s = list.size();
		std::vector<std::shared_ptr<Node>>& nodes = node.m_children;
#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::cout << "\nNodes from " << node << ": " << std::flush;
		}
#endif
		for (unsigned short i {0}; i < s; ++i) {
			UL pos = list[i];
			OthelloBoard b = board;
			//nodes.push_back(std::make_shared<Node>(m_turn, node->m_depth + 1, pos, board, black));
			std::shared_ptr<Node> child = std::make_shared<Node>(node.m_turn, node.m_depth + 1, pos, b, black);
			nodes.push_back(child);
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << *child  << ", " << std::flush;
			}
#endif
		}

		if (nodes.size() == 0) {
			if (node.m_board.hasMove(node.m_black)) {
				//!black has no more move but black has, so !black can't play
				//std::cout << "\nYYYYYYYYYYYYYYY" << std::flush;
				OthelloBoard b = node.m_board;
				nodes.push_back(std::make_shared<Node>(node.m_turn, node.m_depth + 1, 0L, b, black));
			}
		}
		return nodes;

	}
};



} /* namespace othello */

