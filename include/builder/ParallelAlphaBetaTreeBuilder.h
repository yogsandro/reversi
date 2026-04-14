/*
 * AlphaBetaTreeBuilder.h
 *
 *  Created on: Jun 30, 2024
 *      Author: sandro
 */

#ifndef PARALLELALPHABETATREEBUILDER_H_
#define PARALLELALPHABETATREEBUILDER_H_

#include <iostream>
#include <atomic>
#include <mutex>
#include <vector>
#include <deque>
#include <memory>
#include <format>
#include <syncstream>
#include <functional>
#include <concepts>
#include <unordered_map>

#include "../Config.h"
#include "../util/Util.h"
#include "../util/ThreadsafeQueue.h"
#include "../util/RandomEngineHelper.h"
#include "TreeBuilder.h"

#ifdef TBB
#include "oneapi/tbb/concurrent_hash_map.h"
#endif

namespace othello {

template<typename T> class ParallelAlphaBetaTreeBuilder;
template<typename T> class AlphaBetaTreeBuilder;


/**
 * This class is a context for the alpha-beta pruning algorithm. It handles all the
 * aspects/properties to apply the algorithm
 */
template<typename T>
requires std::derived_from<T, othello::TreeNode<T>> // @suppress("Member declaration not found")
class Context {
public:
	/**
	 * The underlying tree node
	 */
	std::shared_ptr<T> m_node;

	/**
	 * The parent context i.e. the context containing the parent node of the node of this context
	 */
	std::shared_ptr<Context<T>> m_parent;

	/**
	 * The candidate context (may vary during the computation of the tree). When the tree is built
	 * it points to the validated context
	 */
	//std::shared_ptr<Context<T>> m_candidate;
	//std::vector<std::shared_ptr<Context<T>>> m_candidates;

	/**
	 *
	 */
	std::vector<std::shared_ptr<Context<T>>> m_children;

//private:
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

	Context(std::shared_ptr<T> node, std::shared_ptr<Context<T>> pParent, unsigned short card = -1) :
		m_node(node),
		m_parent(pParent),
		//m_candidate(nullptr),
		//m_alpha(pParent.get() != nullptr ? pParent.get()->alpha() : std::numeric_limits<short>::min()),
		//m_beta(pParent.get() != nullptr ? pParent.get()->beta() : std::numeric_limits<short>::max()),
		//m_value(node.m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max()),
		m_card(card)
		//m_closed(false),
		//m_count(m_card)
	{
		init(pParent);
	}

	Context(std::shared_ptr<T> node, short alpha, short beta, unsigned short card = -1) :
		m_node(node),
		m_parent(nullptr),
		//m_candidate(nullptr),
		m_alpha(alpha),
		m_beta(beta),
		m_value(node->m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max()),
		m_card(card),
		m_closed(false),
		m_count(m_card) {
	}

	Context() = delete;
	~Context() = default;

	//We need to write this ctor because of the mutex (in case of default : error: use of deleted function 'std::mutex::mutex(const std::mutex&)')
	Context(const Context<T> &other) :
		m_node(other.m_node),
		m_parent(other.m_parent),
		//m_candidate(other.m_candidate),
		m_children (other.m_children),
		m_alpha(other.m_alpha),
		m_beta(other.m_beta),
		m_value(other.m_value),
		m_card(other.m_card),
		m_closed(other.m_closed),
		m_count(other.m_count) {
	};

	Context(Context<T> &&other) = delete;
	Context<T>& operator=(const Context<T> &other) = delete;
	Context<T>& operator=(Context<T> &&other) = delete;


	void init(std::shared_ptr<Context<T>>& pParent) {
		//std::lock_guard<std::mutex> lock(m_mutex);
		m_alpha = pParent != nullptr ? pParent->alpha() : std::numeric_limits<short>::min();
		m_beta = pParent != nullptr ? pParent->beta() : std::numeric_limits<short>::max();
		m_value = m_node->m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();
		m_closed = false;
		m_count = m_card;
	}


	void init() {
		init(m_parent);
		std::for_each(m_children.begin(), m_children.end(), [](std::shared_ptr<Context<T>>& pChildCtxt) {
			//pChildCtxt->init(pChildCtxt->m_parent);
			pChildCtxt->init();
		});
	}

	void open() {
		//if (Config::VERBOSE) std::cout << "\nOpening " << *this <<std::flush;
		//if (m_count > 0) {
			m_closed = false;
		//}
		//if (Config::VERBOSE) std::cout << "\nOpening " << *this <<std::flush;
		std::for_each(m_children.begin(), m_children.end(), [](std::shared_ptr<Context<T>>& pChildCtxt) {
			pChildCtxt->open();
		});
	}

	void setParent(std::shared_ptr<Context<T>> pParent) {
		m_parent = pParent;
	}

	void clear(short depth) {
		if (m_node->m_depth < depth) {
			m_value = m_node->m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();

#ifdef __DEBUG__
			if (Config::VERBOSE) std::cout << "\nClearing at depth " << m_node.m_depth << std::flush;
#endif
			//m_value = std::numeric_limits<short>::min();//
//			m_value = (m_node.m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max());
//			m_count = m_card;
			if (m_node->m_depth + 1 < depth) {
				std::for_each(m_children.begin(), m_children.end(), [&depth](std::shared_ptr<Context<T>>& pChildCtxt) {
					pChildCtxt->clear(depth);
				});
			}
		}
	}

	const std::vector<std::shared_ptr<Context<T>>> leafs() const {
		std::vector<std::shared_ptr<Context<T>>> leafs_;
		if (m_children.size() > 0) {
			std::for_each(m_children.begin(), m_children.end(), [&leafs_](auto pChild) {
				auto cl = pChild->leafs();
				if (!cl.empty()) {
					std::for_each(cl.begin(), cl.end(), [&leafs_](auto pc) {
						leafs_.push_back(pc);
					});
				} else {
					leafs_.push_back(pChild);
				}
			});
		}
		return leafs_;
	}

//	bool hasChildWithNode(T& node) {
//		return std::find_if(m_children.begin(), m_children.end(), [&node](std::shared_ptr<Context<T>>& pChildCtxt) {
//			return pChildCtxt.get()->m_node == node;
//		}) != std::end(m_children);
//	}

	void addChild(std::shared_ptr<Context<T>>& pChildCtxt) {
		m_children.push_back(pChildCtxt);
	}

	std::shared_ptr<Context<T>> getChildContextWithNode(T& node) {
		auto it = std::find_if(m_children.begin(), m_children.end(), [&node](std::shared_ptr<Context<T>>& pChildCtxt) {
			return *(pChildCtxt->m_node) == node;
		});
		return it == std::end(m_children) ? nullptr : *it;
	}

	/**
	 * Climbs the tree to update properties
	 * @param child
	 */
	void updateAll(Context<T>& child, AlphaBetaTreeBuilder<T>& builder, bool decrement = true) {
#ifdef __DEBUG__
		if (Config::VERBOSE) std::cout << "\nUpdateAll context: " << *this << std::flush;
#endif

		//if context is closed before count down, updating it (by passing from
		//children) may decrease the count down of its parent (if it is not closed) and finally may
		//close it (parent of this) avoiding other children to update it
		if (!m_closed) {
			//failSoft(child, builder);
			failHard(child, builder, decrement);
			if (m_closed) {
				//			lock.unlock();
				Context<T> *parent = m_parent.get();
				if (parent) {// && !parent->m_closed) {
					m_parent->updateAll(*this, builder, decrement);
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

	friend std::ostream& operator<<(std::ostream &f, Context<T>& ctxt) {
		short a = ctxt.m_alpha;
		short b = ctxt.m_beta;
		auto pNode = ctxt.m_node;
		bool maximizingPlayer = pNode->m_depth % 2 == 0;

		f << *pNode << ", "
				<< (maximizingPlayer ? "MAX" : "MIN") << ", [\u03B1="
				<< Util::itoa(a) << ", \u03B2=" << Util::itoa(b) << "], v=" << ctxt.m_value << ", closed=" << ctxt.m_closed
				<< ", card=" << ctxt.m_card << "/" << ctxt.m_count;
		return f;
	}

	void setAlpha(short mAlpha) {
		m_alpha = mAlpha;
	}

	void setBeta(short mBeta) {
		m_beta = mBeta;
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
	bool failHard(Context<T>& ctxt, AlphaBetaTreeBuilder<T>& builder, bool decrement = true) {
		short value = ctxt.m_value;

		auto node = m_node;

#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::stringstream ss;
			ss << "START FH - child=";
			ss << *(ctxt.m_node);
			out(value, ss.str());
		}
#endif

		bool maximizingPlayer = node->m_depth % 2 == 0;

//		if (builder.isUseCache()) {
//			builder.store(ctxt);
//		}

		if (value == m_value) {
			//m_candidate = std::make_shared<Context<T>>(ctxt);
			//m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
			//check if this is root context
			if (m_parent == nullptr) {
				builder.m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
			}
		}

		if (maximizingPlayer) {
			if (value > m_value) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value << " > value (" << Util::itoa(m_value) << ") value=" << value << " => [C-" << *(ctxt.m_node) << "]";std::cout << std::flush;
				}
#endif
				m_value = value;
				//m_candidate = std::make_shared<Context<T>>(ctxt); //just keep a reference
				//m_candidates.clear();
				//m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
				//check if this is root context
				if (m_parent == nullptr) {
					builder.m_candidates.clear();
					builder.m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
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
					ss << *(ctxt.m_node);
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
					std::cout << "\n\t" << value << " < value (" << Util::itoa(m_value) << ") value=" <<  value << " => [C-" << *(ctxt.m_node) << "]"; std::cout << std::flush;
				}
#endif
				m_value = value;
				//m_candidate = std::make_shared<Context<T>>(ctxt);
				//m_candidates.clear();
				//m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
				//check if this is root context
				if (m_parent == nullptr) {
					builder.m_candidates.clear();
					builder.m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
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
					ss << *(ctxt.m_node);
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
			ss << *(ctxt.m_node);
			out(value, ss.str());//std::cout << *this << std::format(" U({:d}) value={:s}", value, Util.itoa(m_value));std::cout << std::flush;//, //node.m_id,
		}
#endif

		return false;
	}

	/**
	 * Fail soft version of this algo
	 * @param ctxt
	 * @param builder
	 * @return
	 */
	bool failSoft(Context<T>& ctxt, AlphaBetaTreeBuilder<T>& builder) {
		short value = ctxt.m_value;

		auto node = m_node;

#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::stringstream ss;
			ss << "START FS - child=";
			ss << ctxt.m_node;
			out(value, ss.str());
		}
#endif

		bool maximizingPlayer = node.m_depth % 2 == 0;

		if (builder.isUseCache()) {
			builder.store(ctxt);
		}

		if (value == m_value) {
			if (m_parent == nullptr) {
				builder.m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
			}
		}

		if (maximizingPlayer) {
			if (value > m_value) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value << " > value (" << Util::itoa(m_value) << ") value=" << value << " => [C-" << *(ctxt.m_node) << "]";std::cout << std::flush;
				}
#endif
				m_value = value;
				node->m_value = m_value;
				//m_candidate = std::make_shared<Context<T>>(ctxt); //just keep a reference
				if (m_parent == nullptr) {
					builder.m_candidates.clear();
					builder.m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
				}
			}

			if (m_value <= m_alpha) {
				//m_value is a upper bound for node
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " <= \u03B1 (" << Util::itoa(m_alpha) << ")";std::cout << std::flush;
				}
#endif
				//builder.store(ctxt, 1);
			}

			if (m_value > m_alpha) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " > \u03B1 (" << Util::itoa(m_alpha) << ") => \u03B1=" << m_value;std::cout << std::flush;
				}
#endif
//				if (m_value < m_beta) {
//					builder.store(ctxt, 0);
//				}
				m_alpha = m_value;
			}

			if (m_value >= m_beta) {
				//m_value = m_beta;/////
				//beta cutoff, fail high
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " >= \u03B2 (" << Util::itoa(m_beta) << ") => STOP, \u03B2 cutoff";std::cout << std::flush;
				}
#endif
				//m_value is a lower bound for node
				stop();
//				builder.store(ctxt, -1);
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::stringstream ss;
					ss << "  END FS - child=";
					ss << ctxt.m_node;
					out(value, ss.str());
				}
#endif
				return true;
			}
			decrementCount();
		} else {
			if (value < m_value) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << value << " < value (" << Util::itoa(m_value) << ") value=" <<  value << " => [C-" << *(ctxt.m_node) << "]"; std::cout << std::flush;
				}
#endif
				m_value = value;
				node->m_value = m_value;
				//m_candidate = std::make_shared<Context<T>>(ctxt);
				if (m_parent == nullptr) {
					builder.m_candidates.clear();
					builder.m_candidates.push_back(std::make_shared<Context<T>>(ctxt));
				}
			}

//			if (m_value >= m_beta) {
//				if (Config::VERBOSE) std::cout << "\n\t" << m_value << " >= \u03B2 (" << Util::itoa(m_beta) << ")"; std::cout << std::flush;
//				//m_value is a lower bound for node
//				builder.store(ctxt, -1);
//			}

			if (m_value < m_beta) {
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " < \u03B2 (" << Util::itoa(m_beta) << ") => \u03B2=" << m_value;std::cout << std::flush;
				}
#endif
//				if (m_value > m_alpha) {
//					builder.store(ctxt, 0);
//				}
				m_beta = m_value;
			}

			if (m_value <= m_alpha) {
				//m_value = m_alpha;////
				//alpha cutoff, fail low
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::cout << "\n\t" << m_value << " <= \u03B1 (" << Util::itoa(m_alpha) << ") => STOP, \u03B1 cutoff";std::cout << std::flush;
				}
#endif
				//m_value is an upper bound for node
				stop();
//				builder.store(ctxt);
#ifdef __DEBUG__
				if (Config::VERBOSE) {
					std::stringstream ss;
					ss << "  END FS - child=";
					ss << ctxt.m_node;
					out(value, ss.str());
				}
#endif
				return true;
			}
			decrementCount();
		}
#ifdef __DEBUG__
		if (Config::VERBOSE) {
			std::stringstream ss;
			ss << "  END FS - child=";
			ss << ctxt.m_node;
			out(value, ss.str());//std::cout << *this << std::format(" U({:d}) value={:s}", value, Util.itoa(m_value));std::cout << std::flush;//, //node.m_id,
		}
#endif

		return false;
	}



private:

	/**
	 * Mutex for the threadsafe access
	 */
//PAR	mutable std::mutex m_mutex;
};



template <typename T>
class AlphaBetaTreeBuilder : public TreeBuilder<T> {
	friend class Context<T>;
public:
	AlphaBetaTreeBuilder() = delete;
	AlphaBetaTreeBuilder(Tree<T>& tree, short alpha, short beta, int maxTreeDepth) :
		TreeBuilder<T>(tree, maxTreeDepth),
		m_alpha(alpha),
		m_beta(beta),
		m_sortFunction ([](std::vector<std::shared_ptr<T>>& nodes) {}) {
	}
	virtual ~AlphaBetaTreeBuilder() = default;
	AlphaBetaTreeBuilder(const AlphaBetaTreeBuilder<T> &other) = default;
	AlphaBetaTreeBuilder(AlphaBetaTreeBuilder<T> &&other) = default;
	AlphaBetaTreeBuilder<T>& operator=(const AlphaBetaTreeBuilder<T> &other) = default;
	AlphaBetaTreeBuilder<T>& operator=(AlphaBetaTreeBuilder<T> &&other) = default;

	/**
	 * Get the initial context i.e. the list of all possibilities starting from the
	 * initial state of the underlying tree root node
	 * @return
	 */
	virtual std::vector<std::shared_ptr<Context<T>>> retrieveInitialContext() = 0;

	/**
	 * Main method of this class: it builds the tree
	 * @return
	 */
	virtual void build() override {
		std::vector<std::shared_ptr<Context<T>>> startingContext = retrieveInitialContext();
		startsFrom(startingContext);
	}

	/**
	 * Main method of this class: it builds the tree
	 * @return
	 */
	virtual void startsFrom(std::vector<std::shared_ptr<Context<T>>>& startingContext) {
		//...and push all items in the working queue
		std::for_each(startingContext.begin(), startingContext.end(), [this](auto pCtxt) {
			m_queue.push_back(pCtxt);
		});

		while(!m_queue.empty()) {
//#ifdef __DEBUG__
//			if (Config::VERBOSE) std::cout << "\n==========XXXXX===========\n";
//#endif
			std::shared_ptr<Context<T>> pContext = m_queue.front();
			m_queue.pop_front();
			buildFrom(pContext);
		}
	}

	constexpr long totalNodeCount() const {
		return m_totalNodeCount;
	}

	virtual void sortNodes(std::vector<std::shared_ptr<T>>& list) const {
		if (m_sortFunction) {
			m_sortFunction(list);
		}
	}

	/**
	 *  The first element of the principal variation
	 * @param f
	 * @param node
	 */
	std::shared_ptr<Context<T>>& validated() {
		return m_rootContext;
	}

	constexpr void updateAll(Context<T>& ctxt, bool decrement = true) {
		ctxt.m_parent->updateAll(ctxt, *this, decrement);
	}

	/**
	 * The validated node of the tree
	 * @param f
	 * @param node
	 */
	std::shared_ptr<T> validatedNode() {
		auto pCtxt = validatedContext();
		return pCtxt ? pCtxt->m_node : nullptr;
	}

	/**
	 * The validated context of the tree
	 * @param f
	 * @param node
	 */
	std::shared_ptr<Context<T>> validatedContext() {
		//choose random
		auto& rnd = rnd::RandomEngineHelper::instance();

//		for(auto pCtxt : m_candidates) {
//			std::cout << "\n1-" << *pCtxt;
//		}
		const int n = m_candidates.size();
		return n > 0 ? m_candidates[rnd.uniform_int(n)] : nullptr;
	}

	constexpr short alpha() {
		return this->m_rootContext->alpha();
	}

	constexpr short beta() {
		return this->m_rootContext->alpha();
	}

	constexpr short value() {
		return this->m_rootContext->m_value;
	}

	void setSortFunction(
			std::function<void(std::vector<std::shared_ptr<T>>&)> sortFunction) {
		m_sortFunction = sortFunction;
	}

	void setAlpha(short alpha) {
		m_alpha = alpha;
	}

	void setBeta(short beta) {
		m_beta = beta;
	}

	std::shared_ptr<Context<T>>& rootContext() {
		return m_rootContext;
	}

	void setRootContext(std::shared_ptr<Context<T>> pCtxt) {
		m_rootContext = pCtxt;
		if (pCtxt.get() != nullptr) {
			pCtxt->setParent(nullptr);
		}
	}


protected:
	std::shared_ptr<Context<T>> m_rootContext;
	short m_alpha;
	short m_beta;
	std::function<void(std::vector<std::shared_ptr<T>>&)> m_sortFunction;

	std::deque<std::shared_ptr<Context<T>>> m_queue;

	std::atomic_long m_totalNodeCount;

	/**
	 * @param node
	 */
	virtual void buildFrom(std::shared_ptr<Context<T>>& pCtxt) = 0;

public:
	/**
	 * The candidate context (may vary during the computation of the tree). When the tree is built
	 * it points to the validated context
	 */
	//std::shared_ptr<Context<T>> m_candidate;

	std::vector<std::shared_ptr<Context<T>>> m_candidates;


};



} /* namespace othello */

#endif /* PARALLELALPHABETATREEBUILDER_H_ */
