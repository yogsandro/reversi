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
#include <cassert>
#include <cstddef>
#include <new>

#ifdef __DEBUG__
#include <sstream>
#endif
#include "tree/Tree.h"
#include "builder/TreeBuilder.h"
#include "util/RandomEngineHelper.h"
#include "util/Util.h"
#include "othello/GameRegistry.h"
#include "othello/OthelloBoard.h"

namespace othello {

class AlphaBetaOthelloTreeBuilder;

// minimal C++11 allocator with debug output
template<class Tp>
struct NAlloc
{
    typedef Tp value_type;

    NAlloc() = default;
    template<class T>
    NAlloc(const NAlloc<T>&) {}

    Tp* allocate(std::size_t n)
    {
        m_totalCount += n;
        n *= sizeof(Tp);
    	m_totalMem += n;
        Tp* p = static_cast<Tp*>(::operator new(n));
       // std::cout << "\nallocating " << n << " bytes @ " << p << "," << m_totalCount << std::flush;
        return p;
    }

    void deallocate(Tp* p, std::size_t n)
    {
    	//total -= n;
    	m_totalCount -= n;
    	m_totalMem -= n * sizeof *p;
        //std::cout << "\ndeallocating " << n * sizeof *p << " bytes @ " << p << "," << m_totalCount << std::flush;
        ::operator delete(p);
    }

    static std::atomic_long m_totalCount;
    static std::atomic_long m_totalMem;
};

template<class T>
std::atomic_long NAlloc<T>::m_totalCount = 0;
template<class T>
std::atomic_long NAlloc<T>::m_totalMem = 0;

template<class T, class U>
bool operator==(const NAlloc<T>&, const NAlloc<U>&) { return true; }

template<class T, class U>
bool operator!=(const NAlloc<T>&, const NAlloc<U>&) { return false; }


#ifdef __COMPACT__

#define _VAR_BOOL(name, pos)                                    \
	static constexpr unsigned long name##_mask = 1L << pos;		\
	inline void name(bool new_##name) { 						\
		l = new_##name ? l | name##_mask : l & ~name##_mask;	\
	}															\
	inline bool name() const {									\
		return (name##_mask & l) != 0;							\
	}

#define _UVAR(T, name, v, pos)                                                	\
	static constexpr unsigned long name##_mask = ((unsigned long)v) << pos;		\
	inline void name(T new_##name) { 											\
		l = (((unsigned long)new_##name) << pos) | (l & ~name##_mask);			\
	}                                                                       	\
	inline T name() const {                  									\
		return (name##_mask & l) >> pos;										\
	}

#define _VAR(T, name, v, pos)                                                	\
	static constexpr unsigned long name##_mask = ((unsigned long)v) << pos;		\
	inline void name(T new_##name) { 											\
		l = (((unsigned long)new_##name + 32768) << pos) | (l & ~name##_mask);	\
	}                                                                       	\
	inline T name() const {                  									\
		return ((name##_mask & l) >> pos) - 32768;								\
	}

#define _VAR1(T, name, v, pos)                                                	\
	static constexpr unsigned long name##_mask = ((unsigned long)v) << pos;		\
	inline void name(T new_##name) { 											\
		l = (((unsigned long)new_##name + 1) << pos) | (l & ~name##_mask);		\
	}                                                                       	\
	inline T name() const {                  									\
		return ((name##_mask & l) >> pos) - 1;									\
	}


struct AlphaBetaNode final : public std::enable_shared_from_this<AlphaBetaNode> {
#ifdef __DEBUG__
	unsigned long m_id;
#endif
	std::vector<std::shared_ptr<AlphaBetaNode>> m_children;

	unsigned long l = 0;
	_VAR_BOOL(				closed, 			63);
	_VAR_BOOL(				black, 				62);
	_VAR1(short, 			count, 	0b11111, 	57);
	_VAR1(short, 			card, 	0b11111, 	52);
	_UVAR(unsigned short, 	depth, 	0b1111,		48);
	_VAR(short, 			value, 	0b1111111111111111, 32);
	_VAR(short, 			alpha, 	0b1111111111111111, 16);
	_VAR(short, 			beta, 	0b1111111111111111, 0);


	unsigned long m_pos;

	AlphaBetaNode() = delete;
	~AlphaBetaNode() = default;
	AlphaBetaNode(const AlphaBetaNode &other) = default;
	AlphaBetaNode(AlphaBetaNode &&Node) = default;
	AlphaBetaNode& operator=(const AlphaBetaNode &other) = default;
	AlphaBetaNode& operator=(AlphaBetaNode &&other) = default;

	AlphaBetaNode(std::shared_ptr<AlphaBetaNode> pParent, unsigned short depth_, unsigned long pos, OthelloBoard& board, bool black_) :
		m_children(1, pParent),
		m_pos(pos) {
		depth(depth_);
		black(black_);
		card(-1);
		init(pParent);
	}

	void init(std::shared_ptr<AlphaBetaNode>& pParent) {
//		m_parent = pParent;
		alpha(pParent != nullptr ? pParent->alpha() : std::numeric_limits<short>::min());
		beta(pParent != nullptr ? pParent->beta() : std::numeric_limits<short>::max());
		value(depth() % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max());
		closed(false);
		count(card());
	}

	bool hasChild() const {
		return m_children.size() > 1;
	}

	void addChild(std::shared_ptr<AlphaBetaNode> child) {
		child->depth(depth() + 1);
		m_children.push_back(child);
	}

	std::shared_ptr<AlphaBetaNode> getChild(const std::shared_ptr<AlphaBetaNode>& child) {
		return getChild(*child);
	}

	std::shared_ptr<AlphaBetaNode> getChild(const AlphaBetaNode& node) {
		auto r = std::find_if(m_children.begin() + 1, m_children.end(),
				[&node](std::shared_ptr<AlphaBetaNode> pChild){ return *pChild == node; });
		return r != std::end(m_children) ? *r : nullptr;
	}

	std::shared_ptr<AlphaBetaNode> getPtr() {
		return shared_from_this();
	}

	void init() {
		init(*m_children.begin());
		std::for_each(m_children.begin() + 1, m_children.end(), [](std::shared_ptr<AlphaBetaNode>& pChildCtxt) {
			pChildCtxt->init();
		});
	}

	void open() {
		closed(false);
		std::for_each(m_children.begin() + 1, m_children.end(), [](std::shared_ptr<AlphaBetaNode>& pChild) {
			pChild->open();
		});
	}

	void setParent(std::shared_ptr<AlphaBetaNode> pParent) {
		m_children[0] = pParent;
	}

	void clear(short depth_) {
		if (depth() < depth_) {
			value(depth() % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max());
#ifdef __DEBUG__
			if (Config::VERBOSE) std::cout << "\nClearing at depth " << depth() << std::flush;
#endif
			if (depth() + 1 < depth_) {
				std::for_each(m_children.begin() + 1, m_children.end(), [&depth_](std::shared_ptr<AlphaBetaNode>& pChild) {
					pChild->clear(depth_);
				});
			}
		}
	}

	///Breaks the cyclic dependencies between node with m_parent and m_children fields in
	///order to release memory.
	//The parameter indicates the node child which is not to be freed
	void freeUntil(std::shared_ptr<AlphaBetaNode> pNode = nullptr) {
		std::for_each(m_children.begin() + 1, m_children.end(),
			[&pNode](auto& pChild) {
				if (pNode == nullptr || *pNode != *pChild) {
					pChild->freeUntil();
				}
			}
		);
		m_children.clear();
		//m_children.push_back(nullptr); // parent
	}

	void addChild(std::shared_ptr<AlphaBetaNode>& pChildCtxt) {
		m_children.push_back(pChildCtxt);
	}

	std::shared_ptr<AlphaBetaNode> getChildContextWithNode(AlphaBetaNode& node) {
		auto it = std::find_if(m_children.begin() + 1, m_children.end(),
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
		if (!closed()) {
			//failSoft(child, builder);
			failHard(pChild, candidates, decrement);
			if (closed()) {
				auto parent = m_children[0];
				if (parent.get()) {
					parent->updateAll(getPtr(), candidates, decrement);
				}
			}
		}
	}

	/**
	 *
	 * @return
	 */
	bool update() {
		auto parent = m_children[0];
		if (parent.get()) {
			alpha(parent->alpha());
			beta(parent->beta());
			return true;
		}
		return false;
	}

	/**
	 * Set the cardinality of this context (i.e. the number of children nodes)
	 * @param card
	 */
	void setCard(unsigned short card_) {
		card(card_);
		count(card_);
	}

	void setAlpha(short mAlpha) {
		alpha(mAlpha);
	}

	void setBeta(short mBeta) {
		beta(mBeta);
	}

	void setDepth(unsigned short depth_) {
		depth(depth_);
		std::for_each(m_children.begin() + 1, m_children.end(), [depth_](auto pChild) {
			pChild->setDepth(depth_ + 1);
		});
	}

#ifdef __DEBUG__
	void out(short value_, std::string msg = "") {
		std::cout << "\n" << msg << " - current=" << *this << std::format(", U({:d}) value={:s}", value_, Util::itoa(value())); std::cout << std::flush;
	}
#endif


private:

	void stop() {
		closed(true);
		//std::cout << "*" << std::flush;
#ifdef __DEBUG__
		if (Config::VERBOSE) std::cout << " [STOP]" << std::flush;
#endif
	}

	void decrementCount() {
		count(count() - 1);
		if (count() == 0) {
			closed(true);
		}
	}

	/**
	 * Fail hard version of this algo
	 * @param ctxt
	 * @param builder
	 * @return
	 */
	bool failHard(std::shared_ptr<AlphaBetaNode>& pNode, std::vector<std::shared_ptr<AlphaBetaNode>>& candidates, bool decrement = true) {
		short value_ = pNode->value();

#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::stringstream ss;
			ss << "START FH - child=";
			ss << *pNode;
			out(value_, ss.str());
		}
#endif

		bool maximizingPlayer = depth() % 2 == 0;

//		if (builder.isUseCache()) {
//			builder.store(ctxt);
//		}

		if (value_ == value()) {
			//check if this is root context

			if (m_children[0].get() == nullptr) {
				candidates.push_back(pNode);
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t+" << *pNode << std::flush;
				}
#endif
			}
		}

		if (maximizingPlayer) {
			if (value_ > value()) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value_ << " > value (" << Util::itoa(value()) << ") value=" << value_ << " => [C-" << *pNode << "]";std::cout << std::flush;
				}
#endif
				value(value_);

				//check if this is root context
				if (m_children[0].get() == nullptr) {
					candidates.clear();
					candidates.push_back(pNode);
#ifdef __DEBUG__
					if (Config::VERBOSE) {
						std::cout << "\n\tClear+" << *pNode; std::cout << std::flush;
					}
#endif
				}
			}
			if (value() > beta()) {
				//beta cutoff
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value() << " > \u03B2 (" << Util::itoa(beta()) << ") => STOP, \u03B2 cutoff";std::cout << std::flush;
				}
#endif
				stop();
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::stringstream ss;
					ss << "  END FH - child=";
					ss << *pNode;
					out(value_, ss.str());
				}
#endif
				if (decrement) decrementCount();
				return true;
			}
			if (value() > alpha()) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value() << " > \u03B1 (" << Util::itoa(alpha()) << ") => \u03B1=" << value();std::cout << std::flush;
				}
#endif
				alpha(value());
			}
			if (decrement) decrementCount();
		} else {
			if (value_ < value()) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value_ << " < value (" << Util::itoa(value()) << ") value=" <<  value_ << " => [C-" << *pNode << "]"; std::cout << std::flush;
				}
#endif
				value(value_);

				//check if this is root context
				if (m_children[0].get() == nullptr) {
					candidates.clear();
					candidates.push_back(pNode);
#ifdef __DEBUG__
					if (Config::VERBOSE) {
						std::cout << "\n\tClear+" << *pNode; std::cout << std::flush;
					}
#endif
				}
			}

			if (value() < alpha()) {
				//alpha cutoff
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value() << " < \u03B1 (" << Util::itoa(alpha()) << ") => STOP, \u03B1 cutoff"; std::cout << std::flush;
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
					out(value_, ss.str());
				}
#endif
				return true;
			}

			if (value() < beta()) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value() << " < \u03B2 (" << Util::itoa(beta()) << ") => \u03B2=" << value(); std::cout << std::flush;
				}
#endif
				beta(value());
			}
			if (decrement) decrementCount();
		}

#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::stringstream ss;
			ss << "  END FH - child=";
			ss << *pNode;
			out(value_, ss.str());//std::cout << *this << std::format(" U({:d}) value={:s}", value, Util.itoa(m_value));std::cout << std::flush;//, //node.m_id,
		}
#endif

		return false;
	}

public:

//	void eval() /*override*/ {
//		//Do the move
//		if (m_pos) {
//			/*m_nf = */m_board->flip(m_pos, m_black);
//		}
//	}

	constexpr bool operator==(const AlphaBetaNode& node) const {
		//std::cout << "\n==[" << node << "," << *this << "]==" << std::flush;
		return m_pos == node.m_pos;
	}

	std::shared_ptr<AlphaBetaNode> getChild(UL pos) {
		auto r = std::find_if(m_children.begin() + 1, m_children.end(),
				[pos](auto pChild){ return pChild->m_pos == pos; });
		return r != std::end(m_children) ? *r : nullptr;
	}

	friend std::ostream& operator<<(std::ostream &f, AlphaBetaNode& node) {
		bool maximizingPlayer = node.depth() % 2 == 0;
		f << std::format(" [D{:d} {:d}-{:s} : {:d}]({:d}), {:s}, [\u03B1={:s}, \u03B2={:s}], v={:d}, closed={:b}, card={:d}/{:d}",
				node.depth(),
				OthelloBoard::toStandard(node.m_pos), node.black() ? "Noir" : "Blanc",
				node.value(),
#ifdef __DEBUG__
				node.m_id,
#else
				0,
#endif
				(maximizingPlayer ? "MAX" : "MIN"),
				Util::itoa(node.alpha()), Util::itoa(node.beta()),
				node.value(), node.closed(), node.card(), node.count());
		return f;
	}
};
#else
struct AlphaBetaNode final : public std::enable_shared_from_this<AlphaBetaNode> {
#ifdef __DEBUG__
	unsigned long m_id;
#endif
	std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>> m_children;
	unsigned short m_depth;
	bool m_black;
//	std::shared_ptr<OthelloBoard> m_board;
//	unsigned long m_pos;
	unsigned short m_upos;

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
//	short m_card;

	/**
	 * This flag indicates that this context is closed meaning all children have been processed or
	 * alpha/beta cut
	 */
	bool m_closed;

	/**
	 * Downward count of the proceeded nodes, starts at m_card
	 */
//	short m_count;

	unsigned short m_countcard;


	AlphaBetaNode() = delete;
	~AlphaBetaNode() = default;
	AlphaBetaNode(const AlphaBetaNode &other) = default;
	AlphaBetaNode(AlphaBetaNode &&Node) = default;
	AlphaBetaNode& operator=(const AlphaBetaNode &other) = default;
	AlphaBetaNode& operator=(AlphaBetaNode &&other) = default;

	AlphaBetaNode(std::shared_ptr<AlphaBetaNode> pParent, unsigned short depth, unsigned long pos, OthelloBoard& board, bool black) :
		m_children(1, pParent),
		m_depth(depth),
		m_black(black),
		//m_board(OthelloBoard(board)),
//		m_pos(pos),
		m_upos(Board::convert(pos))
//		m_card(-1)
	{
//		m_board = std::make_shared<OthelloBoard>(board);
		init(pParent);
	}

	void init(std::shared_ptr<AlphaBetaNode>& pParent) {
//		m_parent = pParent;
		m_alpha = pParent != nullptr ? pParent->m_alpha : std::numeric_limits<short>::min();
		m_beta = pParent != nullptr ? pParent->m_beta : std::numeric_limits<short>::max();
		m_value = m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();
		m_closed = false;
//		m_count = m_card;
		m_countcard = 0;

	}

	bool hasChild() const {
		return m_children.size() > 1;
	}

	void addChild(std::shared_ptr<AlphaBetaNode> child) {
		child.get()->m_depth = m_depth + 1;
		m_children.push_back(child);
	}

	std::shared_ptr<AlphaBetaNode> getChild(const std::shared_ptr<AlphaBetaNode>& child) {
		return getChild(*child);
	}

	std::shared_ptr<AlphaBetaNode> getChild(const AlphaBetaNode& node) {
		auto r = std::find_if(m_children.begin() + 1, m_children.end(),
				[&node](std::shared_ptr<AlphaBetaNode> pChild){ return *pChild == node; });
		return r != std::end(m_children) ? *r : nullptr;
	}

	std::shared_ptr<AlphaBetaNode> getPtr() {
		return shared_from_this();
	}

	void init() {
		init(*m_children.begin());
		std::for_each(m_children.begin() + 1, m_children.end(), [](std::shared_ptr<AlphaBetaNode>& pChildCtxt) {
			pChildCtxt->init();
		});
	}

	void open() {
		m_closed = false;
		std::for_each(m_children.begin() + 1, m_children.end(), [](std::shared_ptr<AlphaBetaNode>& pChild) {
			pChild->open();
		});
	}

	void setParent(std::shared_ptr<AlphaBetaNode> pParent) {
		m_children[0] = pParent;
	}

	void clear(short depth) {
		if (m_depth < depth) {
			m_value = m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();
#ifdef __DEBUG__
			if (Config::VERBOSE) std::cout << "\nClearing at depth " << m_depth << std::flush;
#endif
			if (m_depth + 1 < depth) {
				std::for_each(m_children.begin() + 1, m_children.end(), [&depth](std::shared_ptr<AlphaBetaNode>& pChild) {
					pChild->clear(depth);
				});
			}
		}
	}

	///Breaks the cyclic dependencies between node with m_parent and m_children fields in
	///order to release memory.
	//The parameter indicates the node child which is not to be freed
	long freeUntil(std::shared_ptr<AlphaBetaNode> pNode = nullptr) {
		long count = 0;
		std::for_each(m_children.begin() + 1, m_children.end(),
			[&pNode, &count](auto& pChild) {
				if (pNode == nullptr || *pNode != *pChild) {
					count += pChild->freeUntil();
				}
			}
		);
		count += m_children.size() - 1;
		m_children.clear();
		//m_children.push_back(nullptr); // parent
		return count;
	}

	void addChild(std::shared_ptr<AlphaBetaNode>& pChildCtxt) {
		m_children.push_back(pChildCtxt);
	}

	std::shared_ptr<AlphaBetaNode> getChildContextWithNode(AlphaBetaNode& node) {
		auto it = std::find_if(m_children.begin() + 1, m_children.end(),
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
				auto parent = m_children[0];
				if (parent.get()) {
					parent->updateAll(getPtr(), candidates, decrement);
				}
			}
		}
	}

	/**
	 *
	 * @return
	 */
	bool update() {
		auto parent = m_children[0];
		if (parent.get()) {
			m_alpha = parent->alpha();
			m_beta =  parent->beta();
			return true;
		}
		return false;
	}

	/**
	 * Set the cardinality of this context (i.e. the number of children nodes)
	 * @param card
	 */
	void setCard(unsigned short card_) {
//		m_card = card_;
//		m_count = card_;
		m_countcard = (card_ + 1) + 256 * (card_ + 1);
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
		std::for_each(m_children.begin() + 1, m_children.end(), [depth](auto pChild) {
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
		short count = m_countcard % 256 - 1;
		short card = m_countcard / 256 - 1;
		count--;
		m_countcard = (count + 1) + 256 * (card + 1);
		if (count == 0) {
			m_closed = true;
		}
//		m_count--;
//		if (m_count == 0) {
//			m_closed = true;
//		}
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

			if (m_children[0].get() == nullptr) {
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
				if (m_children[0].get() == nullptr) {
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
				if (m_children[0].get() == nullptr) {
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

//	void eval() /*override*/ {
//		//Do the move
//		if (m_pos) {
//			/*m_nf = */m_board->flip(m_pos, m_black);
//		}
//	}

	constexpr bool operator==(const AlphaBetaNode& node) const {
		//std::cout << "\n==[" << node << "," << *this << "]==" << std::flush;
		//return m_pos == node.m_pos;
		return m_upos == node.m_upos;
	}

	std::shared_ptr<AlphaBetaNode> getChild(UL pos) {
		US upos = Board::convert(pos);
		auto r = std::find_if(m_children.begin() + 1, m_children.end(),
				[upos](auto pChild){ return pChild->m_upos == upos; });
		return r != std::end(m_children) ? *r : nullptr;
	}

	friend std::ostream& operator<<(std::ostream &f, AlphaBetaNode& node) {
		bool maximizingPlayer = node.m_depth % 2 == 0;
		short count = node.m_countcard % 256 - 1;
		short card = node.m_countcard / 256 - 1;
		f << std::format(" [D{:d} {:d}-{:s} : {:d}]({:d}), {:s}, [\u03B1={:s}, \u03B2={:s}], v={:d}, closed={:b}, card={:d}/{:d}",
				node.m_depth,
				/*OthelloBoard::toStandard(node.m_pos),*/ node.m_upos, node.m_black ? "Noir" : "Blanc",
				node.m_value,
#ifdef __DEBUG__
				node.m_id,
#else
				0,
#endif
				(maximizingPlayer ? "MAX" : "MIN"),
				Util::itoa(node.m_alpha), Util::itoa(node.m_beta),
//				node.m_value, node.m_closed, node.m_card, node.m_count);
				node.m_value, node.m_closed, card, count);
		return f;
	}

	void alpha(short alpha) {
		m_alpha = alpha;
	}

	void beta(short beta) {
		m_beta = beta;
	}

	bool black() const {
		return m_black;
	}

	void black(bool black) {
		m_black = black;
	}

	short card() const {
		return m_countcard / 256 - 1;
//		return m_card;
	}

	void card(short card) {
		m_countcard = m_countcard % 256 + 256 * (card + 1);
//		m_card = card;
	}

	bool closed() const {
		return m_closed;
	}

	void closed(bool closed) {
		m_closed = closed;
	}

	short count() const {
		return m_countcard % 256 - 1;
//		return m_count;
	}

	void count(short count) {
		//short count = m_countcard % 256 - 1;
		short card = m_countcard / 256;
		m_countcard = count + 1 + 256 * card;
		//m_count = count;
	}

	unsigned short depth() const {
		return m_depth;
	}

	void depth(unsigned short depth) {
		m_depth = depth;
	}

	short value() const {
		return m_value;
	}

	void value(short mValue) {
		m_value = mValue;
	}
};
#endif

class AlphaBetaEvaluationStrategy {
public:
	AlphaBetaEvaluationStrategy(GameRegistry& gameRegistry) : m_gameRegistry(gameRegistry) {};
	virtual ~AlphaBetaEvaluationStrategy() = default;
	AlphaBetaEvaluationStrategy(const AlphaBetaEvaluationStrategy &other) = default;
	AlphaBetaEvaluationStrategy(AlphaBetaEvaluationStrategy &&other) = default;
	AlphaBetaEvaluationStrategy& operator=(const AlphaBetaEvaluationStrategy &other) = default;
	AlphaBetaEvaluationStrategy& operator=(AlphaBetaEvaluationStrategy &&other) = default;

	virtual void eval(OthelloBoard& board, AlphaBetaNode& node) {
		bool black = node.black();
		bool other = !black;

		unsigned short turn = m_gameRegistry.turn();
		unsigned short depth = node.depth();

		short sc = //1000 +
				(3 + (turn + depth > 28 ? 1 : 0)) * (board.count(black) - board.count(other)) +
				5   * (board.border(other) - board.border(black)) +
				10 * (board.mobility(black) - board.mobility(other)) +
				500 * (board.corner(black) - board.corner(other))
				;

		node.value(sc);
	}
private:
	GameRegistry& m_gameRegistry;
};



class AlphaBetaOthelloTree final {
public:
	AlphaBetaOthelloTree(OthelloBoard& board, bool black) :
		m_black(black) {
			OthelloBoard b = board;
			m_root = std::make_shared<AlphaBetaNode>(nullptr, 0, 0L, b, !m_black);
		};
	AlphaBetaOthelloTree() = delete;
	~AlphaBetaOthelloTree() = default;
	AlphaBetaOthelloTree(const AlphaBetaOthelloTree &other) = delete;
//	{
//		m_black = other.m_black;
//		OthelloBoard b;// = *other.m_root.get()->m_board;
//		m_root = std::make_shared<AlphaBetaNode>(nullptr, 0, 0L, b, !m_black);
//	};
	AlphaBetaOthelloTree(AlphaBetaOthelloTree &&other) = delete;

	AlphaBetaOthelloTree& operator=(const AlphaBetaOthelloTree &other) = delete;
//	{
//		if (this != &other) {
//			m_black = other.m_black;
//			OthelloBoard b;// = *other.m_root.get()->m_board;
//			m_root = std::make_shared<AlphaBetaNode>(nullptr, 0, 0L, b, !m_black);
//		}
//		return *this;
//	};

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
	bool m_black;


};

class AlphaBetaOthelloTreeBuilder {
public:
	AlphaBetaOthelloTreeBuilder() = delete;
	AlphaBetaOthelloTreeBuilder(AlphaBetaOthelloTree& tree, short alpha, short beta,
			AlphaBetaEvaluationStrategy& strategy, int maxTreeDepth) :
		m_tree(tree),
		m_maxTreeDepth(maxTreeDepth),
		m_alpha(alpha),
		m_beta(beta),
		m_sortFunction ([](std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>>& nodes) {}),
		m_strategy(strategy),
		m_totalNodeCount(1) {
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
	virtual std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>> retrieveInitialContext(OthelloBoard& board) {
		auto pNode = m_tree.root();

		std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>> contexts;

		if (!pNode->hasChild()) {
			if (pNode->depth() + 1 <= this->m_maxTreeDepth) {
				//std::vector<std::shared_ptr<Node>> pNodes = getNodesFrom(node);

				auto& pNodes = pNode->m_children;
				if (!pNode->hasChild()) {
					getNodesFrom(board, pNode);
				} else {
					sortNodes(pNodes);
				}

				m_rootContext = pNode;
				pNode->alpha(alpha());
				pNode->beta(beta());
				pNode->setCard(pNodes.size() - 1);
//				pNode->m_board = pBoard->shared_from_this();
				contexts = pNodes;
			}
		} else {
			m_rootContext->init();
			contexts = m_rootContext->m_children;
//			for (unsigned int i = 1; i < contexts.size(); ++i) {
//				std::shared_ptr<AlphaBetaNode>& pChild = contexts[i];
//				pChild->m_board = pBoard->shared_from_this();
//			}
		}

		return contexts;
	}

	/**
	 * Main method of this class: it builds the tree
	 * @return
	 */
	virtual void build(OthelloBoard& board) {
		m_candidates.clear();
		//std::shared_ptr<OthelloBoard> pBoard = std::make_shared<OthelloBoard>(board);
		std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>> startingContext = retrieveInitialContext(board);
		startsFrom(board, startingContext);
	}

	/**
	 * Main method of this class: it builds the tree
	 * @return
	 */
	virtual void startsFrom(OthelloBoard& board, std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>>& startingContext) {
		//...and push all items in the working queue
		std::for_each(startingContext.begin() + 1, startingContext.end(), [this](auto pCtxt) {
			m_queue.push_back(pCtxt);
		});

		while(!m_queue.empty()) {
#ifdef __DEBUG__
			if (Config::VERBOSE) std::cout << "\n==========XXX("<< m_totalNodeCount <<")XXX===========";
#endif
			std::shared_ptr<AlphaBetaNode> pContext = m_queue.front();
			m_queue.pop_front();
			buildFrom(board, pContext);
		}
	}

	void setMaxTreeDepth(unsigned short maxTreeDepth) {
		m_maxTreeDepth = maxTreeDepth;
	}

	constexpr long totalNodeCount() const {
		return m_totalNodeCount;
	}

	virtual void sortNodes(std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>>& list) const {
		if (m_sortFunction) {
			m_sortFunction(list);
		}
	}

	void updateAll(std::shared_ptr<AlphaBetaNode>& pNode, std::vector<std::shared_ptr<AlphaBetaNode>>& candidates, bool decrement = true) {
		pNode->m_children[0]->updateAll(pNode, candidates, decrement);
	}

	/**
	 * The validated node of the tree
	 */
	std::shared_ptr<AlphaBetaNode> validated() {
		//choose random
		auto& rnd = rnd::RandomEngineHelper::instance();
//		for(auto pNode : m_candidates) {
//			std::cout << "\n1-" << *pNode;
//		}
		long c = NAlloc<std::shared_ptr<AlphaBetaNode>>::m_totalCount;
//		long g = NAlloc<std::shared_ptr<AlphaBetaNode>>::m_totalMem;
		std::cout << "\n" << m_totalNodeCount << " (" << c << ") nodes," << c * sizeof(AlphaBetaNode) / 1073741824.0 << " GB"/* << " (" << (double) g / c << " bytes/node)"*/ << std::flush;
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
		return m_rootContext->value();
	}

	void setSortFunction(
			std::function<void(std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>>&)> sortFunction) {
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
	virtual void buildFrom(OthelloBoard& board, std::shared_ptr<AlphaBetaNode>& pNode) {
		OthelloBoard b = board;;

#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::cout << "\nBuild from node " << *pNode << std::flush;
		}
#endif

		auto& pParentNode = pNode->m_children[0];
		if (pParentNode != nullptr && pParentNode->closed()) {
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << "\nParent node is closed: " << *pParentNode << std::flush;
			}
#endif
			return;
		}

		if (pNode->closed()) {
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
//			pNode->eval();
//		}


		if (pNode->m_upos) {
			/*m_nf = */b.flip(OthelloBoard::toLong(pNode->m_upos), pNode->black());
		}

		bool proceed = pNode->depth() + 1 <= this->m_maxTreeDepth;

		if (proceed) {
			auto& pNodes = pNode->m_children;
			unsigned short s = pNodes.size() - 1; //remove parent

			if (s == 0) {
				getNodesFrom(b, pNode);
				s = pNodes.size() - 1;//remove parent
			} else {
				sortNodes(pNodes);
			}

#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << "\ns=" << s << std::flush;
			}
#endif
			//still some more move available ?
			proceed = s > 0;

			if (pNode->card() == -1) {
				pNode->setCard(s);
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\ncard: " << *pNode << std::flush;
				}
#endif
			}

			if (proceed) {
				for (unsigned int i = 1; i < pNodes.size(); ++i) {
					std::shared_ptr<AlphaBetaNode>& pChild = pNodes[i];
//				for (std::shared_ptr<AlphaBetaNode>& pChild : pNodes) {
#ifdef __DEBUG__
//						if (Config::VERBOSE) {
//							std::cout << "\nInitializing context: " << *pChild; std::cout << std::flush;
//						}
#endif
						pChild->init(pNode);
#ifdef __DEBUG__
//						if (Config::VERBOSE) {
//							std::cout << "\nInitialized context: " << *pChild; std::cout << std::flush;
//						}
#endif
					//m_totalNodeCount++;
					buildFrom(b, pChild);
					if (pNode->closed()) {
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

			//if (pNode->m_depth > 2) {
				//pNode->m_board = nullptr;
			//}
		}

		if (!proceed) {
			//We've reached a tree leaf
			m_strategy.eval(b, *pNode);
//			if (pNode->m_depth > 1) {
//				pNode->m_board = nullptr;
//			}
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << "\nLEAF - " << *pNode << std::flush;
			}
#endif
			pNode->value((2 * (pNode->depth()%2 - 1) + 1) * pNode->value());
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
	std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>>& getNodesFrom(OthelloBoard& board, std::shared_ptr<AlphaBetaNode>& pNode) {
//		OthelloBoard& board = *pNode->m_board;
		bool black = !pNode->black();
//		const std::vector<UL> list = board.mobilityList(black);
		std::vector<UL> list = board.mobilityListCornersFirst(black);
		const unsigned short s = list.size();
		auto& nodes = pNode->m_children;
#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::cout << "\nNodes from " << *pNode << ": " << std::flush;
		}
#endif
		pNode->m_children.reserve(s + 1);
		for (unsigned short i {0}; i < s; ++i) {
			UL pos = list[i];
			OthelloBoard b = board;
			//nodes.push_back(std::make_shared<Node>(m_turn, node->m_depth + 1, pos, board, black));
			std::shared_ptr<AlphaBetaNode> pChild = std::make_shared<AlphaBetaNode>(pNode, pNode->depth() + 1, pos, b, black);
			nodes.push_back(pChild);
			//nodes.insert(nodes.end(), pChild);
			m_totalNodeCount++;
#ifdef __DEBUG__
			if (Config::VERBOSE) {
				std::cout << *pChild  << ", " << std::flush;
			}
#endif
		}

		if (nodes.size() == 1) {
			if (board.hasMove(!black)) {
				//!black has no more move but black has, so !black can't play, so we add a '0' position move
				//std::cout << "\nYYYYYYYYYYYYYYY" << std::flush;
				OthelloBoard b = board;//*pNode->m_board;
				auto pChild = std::make_shared<AlphaBetaNode>(pNode, pNode->depth() + 1, 0L, b, black);
				nodes.push_back(pChild);
				m_totalNodeCount++;
			}
		}

		list.clear();
		return nodes;

	}

	void free() {
		m_rootContext = nullptr;
		m_candidates.clear();
		m_queue.clear();
	}

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
	std::function<void(std::vector<std::shared_ptr<AlphaBetaNode>, NAlloc<std::shared_ptr<AlphaBetaNode>>>&)> m_sortFunction;

	std::deque<std::shared_ptr<AlphaBetaNode>> m_queue;


	/**
	 * The candidates (may vary during the computation of the tree). When the tree is built
	 * it points to the validated nodes
	 */
	std::vector<std::shared_ptr<AlphaBetaNode>> m_candidates;

	AlphaBetaEvaluationStrategy& m_strategy;

public:
	std::atomic_long m_totalNodeCount;
};



} /* namespace othello */

