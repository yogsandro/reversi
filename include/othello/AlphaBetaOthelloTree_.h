/*
 * OthelloTree.h
 *
 *  Created on: Jun 23, 2024
 *      Author: sandro
 */

#pragma once

#include <deque>
#include <vector>
#include <memory>
#include <algorithm>
#ifdef __DEBUG__
#include <sstream>
#endif
#include "tree/Tree.h"
#include "builder/TreeBuilder.h"
#include "util/RandomEngineHelper.h"
#include "util/Util.h"
#include "OthelloBoard.h"

namespace othello {

class AlphaBetaOthelloTreeBuilder;

struct AlphaBetaNode_ final : public std::enable_shared_from_this<AlphaBetaNode_> {
#ifdef __DEBUG__
	unsigned long m_id;
#endif
	std::vector<std::shared_ptr<AlphaBetaNode_>> m_children;//24
	std::shared_ptr<AlphaBetaNode_> m_parent;//16
	unsigned short m_depth;//2
	unsigned short m_turn;//2
	bool m_black;//1
	OthelloBoard m_board;//32
	unsigned long m_pos;//8
	long r;
//	short m_alpha;//2
//	short m_beta;//2
//	short m_value;//2
//	short m_card;//2
	bool m_closed;//1
	short m_count;//2


	AlphaBetaNode_() = delete;
	~AlphaBetaNode_() = default;
	AlphaBetaNode_(const AlphaBetaNode_ &other) = default;
	AlphaBetaNode_(AlphaBetaNode_ &&Node) = default;
	AlphaBetaNode_& operator=(const AlphaBetaNode_ &other) = default;
	AlphaBetaNode_& operator=(AlphaBetaNode_ &&other) = default;

	AlphaBetaNode_(std::shared_ptr<AlphaBetaNode_> pParent, unsigned short turn, unsigned short depth, unsigned long pos, OthelloBoard& board, bool black) :
		m_parent(pParent),
		m_depth(depth),
		m_turn(turn),
		m_black(black),
		m_board(OthelloBoard(board)),
		m_pos(pos),
		r(0),
//		m_alpha(std::numeric_limits<short>::min()),
//		m_beta(std::numeric_limits<short>::max()),
//		m_value(m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max()),
//		m_card(-1),
		m_closed(false),
		m_count(0)
	{}

};

struct AlphaBetaNode final : public std::enable_shared_from_this<AlphaBetaNode> {
#ifdef __DEBUG__
	unsigned long m_id;
#endif
	//short m_value;
	std::vector<std::shared_ptr<AlphaBetaNode>> m_children;
	std::shared_ptr<AlphaBetaNode> m_parent;
	unsigned short m_depth;

	unsigned short m_turn;
	//short m_score;
	bool m_black;
	OthelloBoard m_board;
	unsigned long m_pos;
	//unsigned short m_nf;
	/**
	 * Alpha value of the alpha-beta pruning
	 */
	short m_alpha;

	/**
	 * Beta value of the alpha-beta pruning
	 */
	short m_beta;

	/**
	 * Value of the node
	 */
	short m_value;

	/**
	 * Number of children nodes
	 */
	short m_card;

	/**
	 * This flag indicates that this context is closed meaning all children have been processed or
	 * alpha/beta cut
	 */
	bool m_closed;

	/**
	 * Downward count of the proceeded nodes, starts at m_card
	 */
	short m_count;


	AlphaBetaNode() = delete;
	~AlphaBetaNode() = default;
	AlphaBetaNode(const AlphaBetaNode &other) = default;
	AlphaBetaNode(AlphaBetaNode &&Node) = default;
	AlphaBetaNode& operator=(const AlphaBetaNode &other) = default;
	AlphaBetaNode& operator=(AlphaBetaNode &&other) = default;

	AlphaBetaNode(std::shared_ptr<AlphaBetaNode> pParent, unsigned short turn, unsigned short depth, unsigned long pos, OthelloBoard& board, bool black) :
		m_parent(pParent),
		m_depth(depth),
		m_turn(turn),
		m_black(black),
		m_board(OthelloBoard(board)),
		m_pos(pos),
		m_card(-1) {
		init(m_parent);
	}

	bool hasChild() const {
		return m_children.size() > 0;
	}

	void addChild(std::shared_ptr<AlphaBetaNode> child) {
		child.get()->m_depth = m_depth + 1;
		m_children.push_back(child);
	}

	std::shared_ptr<AlphaBetaNode> getChild(const std::shared_ptr<AlphaBetaNode>& child) {
		return getChild(*child);
	}

	std::shared_ptr<AlphaBetaNode> getChild(const AlphaBetaNode& node) {
		auto r = std::find_if(m_children.begin(), m_children.end(),
				[&node](std::shared_ptr<AlphaBetaNode> pChild){ return *pChild == node; });
		return r != std::end(m_children) ? *r : nullptr;
	}


	void init(std::shared_ptr<AlphaBetaNode>& pParent) {
//		m_parent = pParent;
		m_alpha = pParent != nullptr ? pParent->m_alpha : std::numeric_limits<short>::min();
		m_beta = pParent != nullptr ? pParent->m_beta : std::numeric_limits<short>::max();
		m_value = m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();
		m_closed = false;
		m_count = m_card;
	}

	std::shared_ptr<AlphaBetaNode> getPtr() {
		return shared_from_this();
	}

	void init() {
		init(m_parent);
		std::for_each(m_children.begin(), m_children.end(), [](std::shared_ptr<AlphaBetaNode>& pChildCtxt) {
			pChildCtxt->init();
		});
	}

	void open() {
		m_closed = false;
		std::for_each(m_children.begin(), m_children.end(), [](std::shared_ptr<AlphaBetaNode>& pChild) {
			pChild->open();
		});
	}

	void setParent(std::shared_ptr<AlphaBetaNode> pParent) {
		m_parent = pParent;
	}

	void clear(short depth) {
		if (m_depth < depth) {
			m_value = m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();
#ifdef __DEBUG__
			if (Config::VERBOSE) std::cout << "\nClearing at depth " << m_depth << std::flush;
#endif
			if (m_depth + 1 < depth) {
				std::for_each(m_children.begin(), m_children.end(), [&depth](std::shared_ptr<AlphaBetaNode>& pChild) {
					pChild->clear(depth);
				});
			}
		}
	}

	///Breaks the cyclic dependencies between node with m_parent and m_children fields in
	///order to release memory.
	//The parameter indicates the node child which is not to be freed
	void freeUntil(std::shared_ptr<AlphaBetaNode> pNode = nullptr) {
		std::for_each(m_children.begin(), m_children.end(),
			[&pNode](auto& pChild) {
				if (pNode == nullptr || *pNode != *pChild) {
					pChild->freeUntil();
				}
			}
		);
		m_children.clear();
		m_parent = nullptr;
	}

	void addChild(std::shared_ptr<AlphaBetaNode>& pChildCtxt) {
		m_children.push_back(pChildCtxt);
	}

	std::shared_ptr<AlphaBetaNode> getChildContextWithNode(AlphaBetaNode& node) {
		auto it = std::find_if(m_children.begin(), m_children.end(),
			[&node](std::shared_ptr<AlphaBetaNode>& pChild) {
				return *pChild == node;
			}
		);
		return it == std::end(m_children) ? nullptr : *it;
	}

	/**
	 * Climbs the tree to update properties
	 * @param child
	 */
	void updateAll(std::shared_ptr<AlphaBetaNode> pChild, std::vector<std::shared_ptr<AlphaBetaNode>>& candidates, bool decrement = true) {
#ifdef __DEBUG__
		if (Config::VERBOSE) std::cout << "\nUpdateAll context: " << *this << std::flush;
#endif

		//if context is closed before count down, updating it (by passing from
		//children) may decrease the count down of its parent (if it is not closed) and finally may
		//close it (parent of this) avoiding other children to update it
		if (!m_closed) {
			//failSoft(child, builder);
			failHard(pChild, candidates, decrement);
			if (m_closed) {
				if (m_parent.get()) {
					m_parent->updateAll(getPtr(), candidates, decrement);
				}
			}
		}
	}

	/**
	 *
	 * @return
	 */
	bool update() {
		if (m_parent.get()) {
			m_alpha = m_parent->alpha();
			m_beta =  m_parent->beta();
			return true;
		}
		return false;
	}

	/**
	 * Set the cardinality of this context (i.e. the number of children nodes)
	 * @param card
	 */
	void setCard(unsigned short card) {
		m_card = card;
		m_count = card;
	}

	short alpha() {
		return m_alpha;
	}

	short beta() {
		return m_beta;
	}

	void setAlpha(short mAlpha) {
		m_alpha = mAlpha;
	}

	void setBeta(short mBeta) {
		m_beta = mBeta;
	}

	void setDepth(unsigned short depth) {
		m_depth = depth;
		std::for_each(m_children.begin(), m_children.end(), [depth](auto pChild) {
			pChild->setDepth(depth + 1);
		});
	}

#ifdef __DEBUG__
	void out(short value, std::string msg = "") {
		std::cout << "\n" << msg << " - current=" << *this << std::format(", U({:d}) value={:s}", value, Util::itoa(m_value)); std::cout << std::flush;
	}
#endif


private:

	void stop() {
		m_closed = true;
		//std::cout << "*" << std::flush;
#ifdef __DEBUG__
		if (Config::VERBOSE) std::cout << " [STOP]" << std::flush;
#endif
	}

	void decrementCount() {
		m_count--;
		if (m_count == 0) {
			m_closed = true;
		}
	}

	/**
	 * Fail hard version of this algo
	 * @param ctxt
	 * @param builder
	 * @return
	 */
	bool failHard(std::shared_ptr<AlphaBetaNode>& pNode, std::vector<std::shared_ptr<AlphaBetaNode>>& candidates, bool decrement = true) {
		short value = pNode->m_value;

#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::stringstream ss;
			ss << "START FH - child=";
			ss << *pNode;
			out(value, ss.str());
		}
#endif

		bool maximizingPlayer = m_depth % 2 == 0;

//		if (builder.isUseCache()) {
//			builder.store(ctxt);
//		}

		if (value == m_value) {
			//check if this is root context
			if (m_parent == nullptr) {
				candidates.push_back(pNode);
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t+" << *pNode << std::flush;
				}
#endif
			}
		}

		if (maximizingPlayer) {
			if (value > m_value) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value << " > value (" << Util::itoa(m_value) << ") value=" << value << " => [C-" << *pNode << "]";std::cout << std::flush;
				}
#endif
				m_value = value;
				//check if this is root context
				if (m_parent == nullptr) {
					candidates.clear();
					candidates.push_back(pNode);
#ifdef __DEBUG__
					if (Config::VERBOSE) {
						std::cout << "\n\tClear+" << *pNode; std::cout << std::flush;
					}
#endif
				}
			}
			if (m_value > m_beta) {
				//beta cutoff
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " > \u03B2 (" << Util::itoa(m_beta) << ") => STOP, \u03B2 cutoff";std::cout << std::flush;
				}
#endif
				stop();
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::stringstream ss;
					ss << "  END FH - child=";
					ss << *pNode;
					out(value, ss.str());
				}
#endif
				if (decrement) decrementCount();
				return true;
			}
			if (m_value > m_alpha) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " > \u03B1 (" << Util::itoa(m_alpha) << ") => \u03B1=" << m_value;std::cout << std::flush;
				}
#endif
				m_alpha = m_value;
			}
			if (decrement) decrementCount();
		} else {
			if (value < m_value) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value << " < value (" << Util::itoa(m_value) << ") value=" <<  value << " => [C-" << *pNode << "]"; std::cout << std::flush;
				}
#endif
				m_value = value;
				//check if this is root context
				if (m_parent == nullptr) {
					candidates.clear();
					candidates.push_back(pNode);
#ifdef __DEBUG__
					if (Config::VERBOSE) {
						std::cout << "\n\tClear+" << *pNode; std::cout << std::flush;
					}
#endif
				}
			}

			if (m_value < m_alpha) {
				//alpha cutoff
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " < \u03B1 (" << Util::itoa(m_alpha) << ") => STOP, \u03B1 cutoff";std::cout << std::flush;
				}
#endif
				//m_value is an upper bound for node
				stop();
				if (decrement) decrementCount();
//				builder.store(ctxt);
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::stringstream ss;
					ss << "  END FH - child=";
					ss << *pNode;
					out(value, ss.str());
				}
#endif
				return true;
			}

			if (m_value < m_beta) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " < \u03B2 (" << Util::itoa(m_beta) << ") => \u03B2=" << m_value;std::cout << std::flush;
				}
#endif
				m_beta = m_value;
			}
			if (decrement) decrementCount();
		}

#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::stringstream ss;
			ss << "  END FH - child=";
			ss << *pNode;
			out(value, ss.str());//std::cout << *this << std::format(" U({:d}) value={:s}", value, Util.itoa(m_value));std::cout << std::flush;//, //node.m_id,
		}
#endif

		return false;
	}

public:

	void eval() /*override*/ {
		//Do the move
		if (m_pos) {
			/*m_nf = */m_board.flip(m_pos, m_black);
		}
	}

	constexpr bool operator==(const AlphaBetaNode& node) const {
		//std::cout << "\n==[" << node << "," << *this << "]==" << std::flush;
		return m_pos == node.m_pos;//m_turn == node.m_turn && m_depth == node.m_depth && m_board == node.m_board;
	}

	std::shared_ptr<AlphaBetaNode> getChild(UL pos) {
		auto r = std::find_if(m_children.begin(), m_children.end(),
				[pos](auto pChild){ return pChild->m_pos == pos; });
		return r != std::end(m_children) ? *r : nullptr;
	}

	friend std::ostream& operator<<(std::ostream &f, AlphaBetaNode& node) {
		bool maximizingPlayer = node.m_depth % 2 == 0;
		f << std::format(" [D{:d} {:d}-{:s} : {:d}]({:d}), {:s}, [\u03B1={:s}, \u03B2={:s}], v={:d}, closed={:b}, card={:d}/{:d}",
				node.m_depth,
				node.m_board.toStandard(node.m_pos), node.m_black ? "Noir" : "Blanc",
				node.m_value,
#ifdef __DEBUG__
				node.m_id,
#else
				0,
#endif
				(maximizingPlayer ? "MAX" : "MIN"),
				Util::itoa(node.m_alpha), Util::itoa(node.m_beta),
				node.m_value, node.m_closed, node.m_card, node.m_count);
		return f;
	}
};

class AlphaBetaOthelloTree final /*: public Tree<AlphaBetaNode>*/ {
public:
	AlphaBetaOthelloTree(OthelloBoard& board, bool black, unsigned short turn) :
//		Tree(),
		m_turn(turn),
		m_black(black) {
			OthelloBoard b = board;
			m_root = std::make_shared<AlphaBetaNode>(nullptr, m_turn, 0, 0L, b, !m_black);
		};
	AlphaBetaOthelloTree() = delete;
	~AlphaBetaOthelloTree() = default;
	AlphaBetaOthelloTree(const AlphaBetaOthelloTree &other) {
		m_black = other.m_black;
		m_turn = other.m_turn;
		OthelloBoard b = other.m_root.get()->m_board;
		m_root = std::make_shared<AlphaBetaNode>(nullptr, m_turn, 0, 0L, b, !m_black);
	};
	AlphaBetaOthelloTree(AlphaBetaOthelloTree &&other) = default;

	AlphaBetaOthelloTree& operator=(const AlphaBetaOthelloTree &other) {
		if (this != &other) {
			m_black = other.m_black;
			m_turn = other.m_turn;
			OthelloBoard b = other.m_root.get()->m_board;
			m_root = std::make_shared<AlphaBetaNode>(nullptr, m_turn, 0, 0L, b, !m_black);
		}
		return *this;
	};

	void setRoot(std::shared_ptr<AlphaBetaNode> root) {
		m_root = root;
		m_root->setDepth(0);
	}

	const std::shared_ptr<AlphaBetaNode> root() const {
		return m_root;
	}

	void free() {
		m_root->freeUntil();
	}


protected:
	std::shared_ptr<AlphaBetaNode> m_root;
	unsigned short m_turn;
	bool m_black;


};

class AlphaBetaOthelloTreeBuilder /*: public TreeBuilder<AlphaBetaNode> */{
public:
	AlphaBetaOthelloTreeBuilder() = delete;
	AlphaBetaOthelloTreeBuilder(AlphaBetaOthelloTree& tree, short alpha, short beta,
			EvaluationStrategy<AlphaBetaNode>& strategy, int maxTreeDepth) :
		m_tree(tree),
		m_maxTreeDepth(maxTreeDepth),
		m_alpha(alpha),
		m_beta(beta),
		m_sortFunction ([](std::vector<std::shared_ptr<AlphaBetaNode>>& nodes) {}),
		m_strategy(strategy) {
	}
	virtual ~AlphaBetaOthelloTreeBuilder() = default;
	AlphaBetaOthelloTreeBuilder(const AlphaBetaOthelloTreeBuilder &other) = default;
	AlphaBetaOthelloTreeBuilder(AlphaBetaOthelloTreeBuilder &&other) = default;
	AlphaBetaOthelloTreeBuilder& operator=(const AlphaBetaOthelloTreeBuilder &other) = default;
	AlphaBetaOthelloTreeBuilder& operator=(AlphaBetaOthelloTreeBuilder &&other) = default;

	/**
	 * Get the initial context i.e. the list of all possibilities starting from the
	 * initial state of the underlying tree root node
	 * @return
	 */
	virtual std::vector<std::shared_ptr<AlphaBetaNode>> retrieveInitialContext() {
		auto pNode = m_tree.root();

		std::vector<std::shared_ptr<AlphaBetaNode>> contexts;

		if (!pNode->hasChild()) {
			if (pNode->m_depth + 1 <= this->m_maxTreeDepth) {
				//std::vector<std::shared_ptr<Node>> pNodes = getNodesFrom(node);

				std::vector<std::shared_ptr<AlphaBetaNode>>& pNodes = pNode->m_children;
				if (!pNode->hasChild()) {
					getNodesFrom(pNode);
				} else {
					sortNodes(pNodes);
				}

				m_rootContext = pNode;
				pNode->m_alpha = m_alpha;
				pNode->m_beta = m_beta;
				pNode->setCard(pNodes.size());

				contexts = pNodes;
//				m_rootContext = std::make_shared<AlphaBetaNode>(m_alpha, m_beta, pNodes.size());
//
//				for (std::shared_ptr<AlphaBetaNode>& pChild : pNodes) {
//					std::shared_ptr<AlphaBetaNode> pChildCtxt = std::make_shared<AlphaBetaNode>(pChild, m_rootContext);
//					contexts.push_back(pChildCtxt);
//					m_rootContext->m_children.push_back(pChildCtxt);
//				}
			}
		} else {
			m_rootContext->init();
			contexts = m_rootContext->m_children;
//			for (auto pChildCtxt : m_rootContext->m_children) {
//				contexts.push_back(pChildCtxt);
//			}
		}

		return contexts;
	}

	/**
	 * Main method of this class: it builds the tree
	 * @return
	 */
	virtual void build() /*override*/ {
		std::vector<std::shared_ptr<AlphaBetaNode>> startingContext = retrieveInitialContext();
		startsFrom(startingContext);
	}

	/**
	 * Main method of this class: it builds the tree
	 * @return
	 */
	virtual void startsFrom(std::vector<std::shared_ptr<AlphaBetaNode>>& startingContext) {
		//...and push all items in the working queue
		std::for_each(startingContext.begin(), startingContext.end(), [this](auto pCtxt) {
			m_queue.push_back(pCtxt);
		});

		while(!m_queue.empty()) {
#ifdef __DEBUG__
			if (Config::VERBOSE) std::cout << "\n==========XXX("<< m_totalNodeCount <<")XXX===========";
#endif
			std::shared_ptr<AlphaBetaNode> pContext = m_queue.front();
			m_queue.pop_front();
			buildFrom(pContext);
		}
	}

	void setMaxTreeDepth(unsigned short maxTreeDepth) {
		m_maxTreeDepth = maxTreeDepth;
	}

	constexpr long totalNodeCount() const {
		return m_totalNodeCount;
	}

	virtual void sortNodes(std::vector<std::shared_ptr<AlphaBetaNode>>& list) const {
		if (m_sortFunction) {
			m_sortFunction(list);
		}
	}

	void updateAll(std::shared_ptr<AlphaBetaNode>& pNode, std::vector<std::shared_ptr<AlphaBetaNode>>& candidates, bool decrement = true) {
		pNode->m_parent->updateAll(pNode, candidates, decrement);
	}

	/**
	 * The validated node of the tree
	 */
	std::shared_ptr<AlphaBetaNode> validated() {
		//choose random
		auto& rnd = rnd::RandomEngineHelper::instance();
		for(auto pNode : m_candidates) {
			std::cout << "\n1-" << *pNode;
		}
		std::cout << "\n" << m_totalNodeCount;
		const int n = m_candidates.size();
		return n > 0 ? m_candidates[rnd.uniform_int(n)] : nullptr;
	}

	constexpr short alpha() {
		return m_rootContext->alpha();
	}

	constexpr short beta() {
		return m_rootContext->alpha();
	}

	constexpr short value() {
		return m_rootContext->m_value;
	}

	void setSortFunction(
			std::function<void(std::vector<std::shared_ptr<AlphaBetaNode>>&)> sortFunction) {
		m_sortFunction = sortFunction;
	}

	void setAlpha(short alpha) {
		m_alpha = alpha;
	}

	void setBeta(short beta) {
		m_beta = beta;
	}

	std::shared_ptr<AlphaBetaNode>& rootContext() {
		return m_rootContext;
	}

	void setRootContext(std::shared_ptr<AlphaBetaNode> pCtxt) {
		m_rootContext = pCtxt;
		if (pCtxt.get() != nullptr) {
			pCtxt->setParent(nullptr);
		}
	}

	/**
	 * @param node
	 */
	virtual void buildFrom(std::shared_ptr<AlphaBetaNode>& pNode) {
#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::cout << "\nBuild from node " << *pNode << std::flush;
		}
#endif

		auto& pParentNode = pNode->m_parent;
		if (pParentNode != nullptr && pParentNode->m_closed) {
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << "\nParent node is closed: " << *pParentNode << std::flush;
			}
#endif
			return;
		}

		if (pNode->m_closed) {
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
			std::vector<std::shared_ptr<AlphaBetaNode>>& pNodes = pNode->m_children;
			unsigned short s = pNodes.size();

			if (s == 0) {
				getNodesFrom(pNode);
				s = pNodes.size();
			} else {
				sortNodes(pNodes);
			}

			//still some more move available ?
			proceed = s > 0;

			if (proceed) {
				if (pNode->m_card == -1) {
					pNode->setCard(s);
#ifdef __DEBUG__
					if (Config::VERBOSE) {
						std::cout << "\ncard: " << *pNode << std::flush;
					}
#endif
				}
				for (std::shared_ptr<AlphaBetaNode>& pChild : pNodes) {
//#ifdef __DEBUG__
//						if (Config::VERBOSE) {
//							std::cout << "\nInitializing context: " << *pChild; std::cout << std::flush;
//						}
//#endif
						pChild->init(pNode);
//#ifdef __DEBUG__
//						if (Config::VERBOSE) {
//							std::cout << "\nInitialized context: " << *pChild; std::cout << std::flush;
//						}
//#endif
					m_totalNodeCount++;
					buildFrom(pChild);
					if (pNode->m_closed) {
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
			m_strategy.eval(*pNode);
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << "\nLEAF - " << *pNode << std::flush;
			}
#endif
			pNode->m_value = (2 * (pNode->m_depth%2 - 1) + 1) * pNode->m_value;
			//if parent context of ctxt is closed before count down, updating it (by passing here from
			//children) may decrease the count down of its parent (if it is not closed) and finally may
			//close it (parent of parent of ctxt) avoiding other children to update it
			updateAll(pNode, m_candidates);
		}
		//std::cout << "\nbuildFrom " << *node << " done" << std::flush;
//		} catch (...) {
//			std::cout << "Exception caught." << std::flush;
//
//		}
	}

	/**
	 *
	 * @param board
	 * @param black
	 * @return
	 */
	std::vector<std::shared_ptr<AlphaBetaNode>>& getNodesFrom(std::shared_ptr<AlphaBetaNode>& pNode) {
		OthelloBoard& board = pNode->m_board;
		bool black = !pNode->m_black;
//		const std::vector<UL> list = board.mobilityList(black);
		const std::vector<UL> list = board.mobilityListCornersFirst(black);
		const unsigned short s = list.size();
		std::vector<std::shared_ptr<AlphaBetaNode>>& nodes = pNode->m_children;
#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::cout << "\nNodes from " << *pNode << ": " << std::flush;
		}
#endif
		for (unsigned short i {0}; i < s; ++i) {
			UL pos = list[i];
			OthelloBoard b = board;
			//nodes.push_back(std::make_shared<Node>(m_turn, node->m_depth + 1, pos, board, black));
			std::shared_ptr<AlphaBetaNode> pChild = std::make_shared<AlphaBetaNode>(pNode, pNode->m_turn, pNode->m_depth + 1, pos, b, black);
			nodes.push_back(pChild);
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << *pChild  << ", " << std::flush;
			}
#endif
		}

		if (nodes.size() == 0) {
			//if (pNode->m_board.hasMove(pNode->m_black)) {
				//!black has no more move but black has, so !black can't play, so we add a '0' position move
				//std::cout << "\nYYYYYYYYYYYYYYY" << std::flush;
				OthelloBoard b = pNode->m_board;
				auto pChild = std::make_shared<AlphaBetaNode>(pNode, pNode->m_turn, pNode->m_depth + 1, 0L, b, black);
				nodes.push_back(pChild);
			//}
		}
		return nodes;

	}

	void free() {
		m_rootContext = nullptr;
		m_candidates.clear();
		m_queue.clear();
	}

public:
	std::atomic_long m_totalNodeCount;

protected:
	/**
	 * The result tree
	 */
	AlphaBetaOthelloTree& m_tree;

	/**
	 * Max depth of the tree
	 */
	unsigned short m_maxTreeDepth;


	std::shared_ptr<AlphaBetaNode> m_rootContext;

	short m_alpha;
	short m_beta;
	std::function<void(std::vector<std::shared_ptr<AlphaBetaNode>>&)> m_sortFunction;

	std::deque<std::shared_ptr<AlphaBetaNode>> m_queue;



	/**
	 * The candidates (may vary during the computation of the tree). When the tree is built
	 * it points to the validated nodes
	 */
	std::vector<std::shared_ptr<AlphaBetaNode>> m_candidates;

	EvaluationStrategy<AlphaBetaNode>& m_strategy;

};


} /* namespace othello */

