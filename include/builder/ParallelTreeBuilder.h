/*
 * ParallelTreeBuilder.h
 *
 *  Created on: Mar 26, 2025
 *      Author: sandro
 */

#ifndef PARALLELTREEBUILDER_H_
#define PARALLELTREEBUILDER_H_

#include <iostream>
#include <atomic>
#include <mutex>
#include <vector>
#include <memory>
#include <format>
#include <syncstream>
#include <functional>
#include <concepts>
#include <unordered_map>

#include "../Config.h"
#include "../util/Util.h"
#include "../util/ThreadsafeQueue.h"
#include "TreeBuilder.h"

#ifdef TBB
#include "oneapi/tbb/concurrent_hash_map.h"
#endif

namespace othello {

template<typename T>
class ParallelTreeBuilder;


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
	T& m_node;

	/**
	 * The parent context i.e. the context containing the parent node of the node of this context
	 */
	std::shared_ptr<Context<T>> m_parent;

	/**
	 * The candidate context (may vary during the computation of the tree). When the tree is built
	 * it points to the validated context
	 */
	std::shared_ptr<Context<T>> m_candidate;

	/**
	 *
	 */
	std::vector<std::shared_ptr<Context<T>>> m_children;

	/**
	 * Value of the node
	 */
	short m_value;

	/**
	 * Number of children nodes
	 */
	short m_card;


	Context(T& node, std::shared_ptr<Context<T>> pParent, unsigned short card = -1) :
		m_node(node),
		m_parent(pParent),
		m_candidate(nullptr),
		m_card(card)
	{
		init(pParent);
	}

	Context(T& node, short alpha, short beta, unsigned short card = -1) :
		m_node(node),
		m_parent(nullptr),
		m_candidate(nullptr),
		m_value(node.m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max()),
		m_card(card)
	{
	}

	Context() = delete;
	~Context() = default;

	//We need to write this ctor because of the mutex (in case of default : error: use of deleted function 'std::mutex::mutex(const std::mutex&)')
	Context(const Context<T> &other) :
		m_node(other.m_node),
		m_parent(other.m_parent),
		m_candidate(other.m_candidate),
		m_value(other.m_value),
		m_card(other.m_card),
		m_children (other.m_children) {
	};

	Context(Context<T> &&other) = delete;
	Context<T>& operator=(const Context<T> &other) = delete;
	Context<T>& operator=(Context<T> &&other) = delete;


	void init(std::shared_ptr<Context<T>> pParent) {
		m_value = m_node.m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();
	}


	void clear(short depth) {
		if (m_node.m_depth < depth) {
			m_value = m_node.m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max();

			if (Config::VERBOSE) std::cout << "\nClearing at depth " << m_node.m_depth << std::flush;
			//m_value = std::numeric_limits<short>::min();//
//			m_value = (m_node.m_depth % 2 == 0 ? std::numeric_limits<short>::min() : std::numeric_limits<short>::max());
//			m_count = m_card;
			if (m_node.m_depth + 1 < depth) {
				std::for_each(m_children.begin(), m_children.end(), [&depth](std::shared_ptr<Context<T>>& pChildCtxt) {
					pChildCtxt.get()->clear(depth);
				});
			}
		}
	}

	void addChild(std::shared_ptr<Context<T>> pChildCtxt) {
		std::lock_guard<std::mutex> lock(m_mutex);
		m_children.push_back(pChildCtxt);
	}

	std::shared_ptr<Context<T>> getChildContextWithNode(T& node) {
		std::lock_guard<std::mutex> lock(m_mutex);
		auto it = std::find_if(m_children.begin(), m_children.end(), [&node](std::shared_ptr<Context<T>>& pChildCtxt) {
			return pChildCtxt.get()->m_node == node;
		});
		return it == std::end(m_children) ? nullptr : *it;
	}

	/**
	 * Set the cardinality of this context (i.e. the number of children nodes)
	 * @param card
	 */
	void setCard(unsigned short card) {
		m_card = card;
	}

	friend std::ostream& operator<<(std::ostream &f, Context<T>& ctxt) {
		T& node = ctxt.m_node;
		bool maximizingPlayer = node.m_depth % 2 == 0;

		f << node << ", "
				<< (maximizingPlayer ? "MAX" : "MIN") << ", v=" << ctxt.m_value << ", card=" << ctxt.m_card;
		return f;
	}

	void out(short value, std::string msg = "") {
		std::cout << "\n" << msg << " - current=" << *this << std::format(", U({:d}) value={:s}", value, Util::itoa(m_value)); std::cout << std::flush;
	}


private:

	void stop() {
		//std::cout << "*" << std::flush;
		if (Config::VERBOSE) std::cout << " [STOP]" << std::flush;
	}


private:
	/**
	 * Mutex for the threadsafe access
	 */
	mutable std::mutex m_mutex;
};


template <typename T>
class ParallelTreeBuilder : public TreeBuilder<T> {
public:
	ParallelTreeBuilder() = delete;
	ParallelTreeBuilder(Tree<T>& tree, unsigned short n = std::thread::hardware_concurrency()) :
		TreeBuilder<T>(tree, std::numeric_limits<unsigned short>::max()),
		m_maxThreads(n)
	{
	}
	virtual ~ParallelTreeBuilder() = default;
	ParallelTreeBuilder(const ParallelTreeBuilder<T> &other) = default;
	ParallelTreeBuilder(ParallelTreeBuilder<T> &&other) = default;
	ParallelTreeBuilder<T>& operator=(const ParallelTreeBuilder<T> &other) = default;
	ParallelTreeBuilder<T>& operator=(ParallelTreeBuilder<T> &&other) = default;

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
		std::for_each(startingContext.begin(), startingContext.end(), [this](std::shared_ptr<Context<T>>& pCtxt) {
			m_queue.push(pCtxt);
		});

		startThreads();
	}

	constexpr long totalNodeCount() const {
		return m_totalNodeCount;
	}

	/**
	 *  The first element of the principal variation
	 * @param f
	 * @param node
	 */
	std::shared_ptr<Context<T>>& validated() {
		return m_rootContext;
	}

	/**
	 * The validated node of the tree
	 * @param f
	 * @param node
	 */
	T& validatedNode() {
		return this->m_rootContext.get()->m_candidate.get()->m_node;
	}

	constexpr short value() {
		return this->m_rootContext.get()->m_value;
	}

	std::shared_ptr<Context<T>>& rootContext() {
		return m_rootContext;
	}


protected:
	std::shared_ptr<Context<T>> m_rootContext;

	/**
	 * Max number of thread (by default the number of available threads of the system)
	 */
	unsigned short m_maxThreads;

	/**
	 * A threadsafe queue
	 */
	ts_queue<Context<T>> m_queue;


	std::atomic_long m_totalNodeCount;

	/**
	 * Each thread starts from a node and builds child nodes.
	 * @param node
	 * @param nodeCount
	 * @param notProcessedNodes
	 */
	virtual void buildFrom(std::shared_ptr<Context<T>>& pEnv) = 0;

	/**
	 * This function is started by each thread
	 */
	void workerThread() {
		while(!m_queue.empty()) {
			//get a node from the queue...
			auto opt = m_queue.wait_and_pop(250);
			if (opt.has_value()) {
				std::shared_ptr<Context<T>> pContext = opt.value();
				std::vector<std::shared_ptr<Context<T>>> notProcessed;
				unsigned int nodeCount = 0;

				if (Config::VERBOSE) std::cout << "\n==========XXXXX===========\n";
				buildFrom(pContext, nodeCount, notProcessed);

				if (notProcessed.size() > 0) {
					std::for_each(notProcessed.begin(), notProcessed.end(), [this](auto& pCtxt) {
						m_queue.push(pCtxt);
					});
				}
			}
		}
	}


private:
	/**
	 * Mutex for the threadsafe access
	 */
	mutable std::mutex m_mutex;

	void startThreads() {
		//build the threads that will fetch the elements of the queue and build the tree from them
		std::vector<std::thread> threads;
		try {
			for(unsigned i {0}; i < m_maxThreads; ++i) {
				threads.push_back(std::thread(&ParallelTreeBuilder::workerThread, this));
			}

			//join threads
			std::for_each(threads.begin(), threads.end(), [](std::thread& t) { if (t.joinable()) t.join(); });
			//std::cout << "\nDone." << std::flush;
		} catch (...) {
			//TODO rethrow real exception
			std::cout << "Exception caught." << std::flush;

		}
	}



};



} /* namespace othello */

#endif /* PARALLELTREEBUILDER_H_ */
