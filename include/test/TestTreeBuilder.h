/*
 * TestTreeBuilder.h
 *
 *  Created on: Jun 23, 2024
 *      Author: sandro
 */

#pragma once

#include <algorithm>

#include "builder/ParallelAlphaBetaTreeBuilder.h"
#include "tree/TreeLevelVisitor.h"
#include "othello/eval/EvaluationStrategy.h"

namespace othello {

struct TestNode : public TreeNode<TestNode> {
public :
	bool m_black;
	std::string m_label;

	TestNode() = delete;
	~TestNode() = default;
	TestNode(const TestNode &other) = default;
	TestNode(TestNode &&Node) = default;
	TestNode& operator=(const TestNode &other) = default;
	TestNode& operator=(TestNode &&other) = default;

	TestNode (std::string label, bool black) :
		TestNode(label, 0, black, 0) {
	}

	TestNode (std::string label, bool black, int score) :
		TestNode(label, 0, black, score) {
	}

	TestNode (std::string label, unsigned short depth, bool black) :
		TestNode(label, depth, black, 0) {
	}

	TestNode (std::string label, unsigned short depth, bool black, int score) :
		TreeNode(depth, score),
		m_black(black),
		m_label(label) {
	}

	void eval() override {
		;
	}

	constexpr bool operator==(const TestNode& node) const {
		return m_label == node.m_label;
	}

	friend std::ostream& operator<<(std::ostream &f, const TestNode& node) {
		f << std::format(" [{:s} D{:d} {:s} : {:d}]({:d})", node.m_label, node.m_depth,
				node.m_black ? "Noir" : "Blanc", node.m_value, node.hash());
		return f;
	}

};

class TestTree final : public Tree<TestNode> {
public:
	TestTree(bool black, std::shared_ptr<TestNode> root) :
		Tree(root),
		m_black(black) {
	};
	TestTree() = delete;
	~TestTree() = default;
	TestTree(const TestTree &other) = default;
	TestTree(TestTree &&other) = default;
	//TestTree& operator=(const TestTree &other) = default;
	TestTree& operator=(TestTree &&other) = default;

	virtual TestTree& operator=(const TestTree &other) {
		if (this != &other) {
			TestNode t = *(other.m_root.get());
			m_root = std::make_shared<TestNode>(t);
		}
		return *this;
	};

//	virtual void buildFrom(std::shared_ptr<TestNode> pNode) override {
//		TestNode& node = *(pNode.get());
//		if (node.m_depth + 1 <= m_maxTreeDepth) {
//			bool black = !node.m_black;
//			std::for_each(node.m_children.begin(), node.m_children.end(), [this](std::shared_ptr<TestNode>& pChild) {
//				TestNode& child = *dynamic_cast<TestNode*>(pChild.get());
//				buildFrom(std::make_shared<TestNode>(child));
//			});
//		}
//	}



protected:
	bool m_black;

};

class TestEvaluationStrategy : public EvaluationStrategy<TestNode>
 {
public:
	TestEvaluationStrategy(unsigned int maxDepth) :
		EvaluationStrategy(),
		m_maxDepth(maxDepth) {
	}

	~TestEvaluationStrategy() = default;
	TestEvaluationStrategy(const TestEvaluationStrategy &other) = default;
	TestEvaluationStrategy(TestEvaluationStrategy &&other) = default;
	TestEvaluationStrategy& operator=(const TestEvaluationStrategy &other) = default;
	TestEvaluationStrategy& operator=(TestEvaluationStrategy &&other) = default;

	virtual void eval(TestNode& node) override {};

	const std::pair<unsigned short, short>& getResult() const {
		return m_result;
	}

private:
	std::pair<unsigned short, short> m_result;
	unsigned int m_maxDepth;

};



class TestTreeBuilder final : public AlphaBetaTreeBuilder<TestNode> {
public:

	TestTreeBuilder() = delete;
	~TestTreeBuilder() = default;
	TestTreeBuilder(const TestTreeBuilder &other) = default;
	TestTreeBuilder(TestTreeBuilder &&other) = default;
	TestTreeBuilder& operator=(const TestTreeBuilder &other) = default;
	TestTreeBuilder& operator=(TestTreeBuilder &&other) = default;

	TestTreeBuilder(Tree<TestNode>& tree, short alpha, short beta, EvaluationStrategy<TestNode>& strategy,
		int maxNodes, int maxDepth, unsigned short n = std::thread::hardware_concurrency()) :
			AlphaBetaTreeBuilder(tree, alpha, beta, maxDepth) {
	};

	virtual std::vector<std::shared_ptr<Context<TestNode>>> retrieveInitialContext() override {
		std::vector<std::shared_ptr<Context<TestNode>>> context;

//		TreeLevelVisitor<TestNode> v(0);
//		m_tree.accept(v);
//		std::vector<std::reference_wrapper<TestNode>>& result = v.getNodes();
//		std::for_each(result.begin(), result.end(), [&context] (std::reference_wrapper<TestNode> nodeRef) {
//			TestNode& node = nodeRef.get();
//			context.push_back(std::make_shared<Context<TestNode>>(node, nullptr, node.m_children.size()));
//		});
//		m_rootContext = std::make_shared<Context<TestNode>>(*(m_tree.root().get()), nullptr, m_tree.root().get()->m_children.size());
		m_rootContext = std::make_shared<Context<TestNode>>(m_tree.root(), m_alpha, m_beta, m_tree.root().get()->m_children.size());
		context.push_back(m_rootContext);
		return context;
	}

	void buildFrom(std::shared_ptr<Context<TestNode>>& pCtxt) override {
		try {
		Context<TestNode>& ctxt = *(pCtxt.get());
		auto pNode = ctxt.m_node;

		if (verbose) std::cout  << "\nExploring node '" << *pNode << std::flush;

//		if (auto opt = retrieve(ctxt); opt.has_value()) {
//			auto pair = opt.value();
//			short lowerBound = pair.first;
//			short upperBound = pair.second;
//			if (verbose) std::cout  << "\nR(" << lowerBound << ", " << upperBound << ")" << std::flush;
//
//			if (lowerBound >= ctxt.beta()) {
//				ctxt.m_closed = true;
//				if (verbose) {
//					std::stringstream ss;
//					ss << " " << lowerBound << ">= \u03B2 (" << ctxt.beta() << ")";
//					ctxt.out(ctxt.m_value, ss.str());
//				}
//				return;
//			} else if (upperBound <= ctxt.alpha()) {
//				ctxt.m_closed = true;
//				if (verbose) {
//					std::stringstream ss;
//					ss << " " << upperBound << "<= \u03B1 (" << ctxt.alpha() << ")";
//					ctxt.out(ctxt.m_value, ss.str());
//				}
//				return;
//			}
//
//			if (lowerBound > ctxt.alpha()) {
//				ctxt.setAlpha(lowerBound);
//			}
//
//			if (upperBound < ctxt.beta()) {
//				ctxt.setBeta(upperBound);
//			}
//		}

		ctxt.update();

		if (ctxt.m_closed) {
			if (verbose) std::cout  << "\nNode '" << *pNode << " is closed" << std::flush;
			return;
		}

		pNode->eval();
		//m_strategy.eval(node);

		//std::cout << "\nbuildFrom " << *node << std::flush;
		if (pNode->m_depth + 1 <= m_maxTreeDepth) {
			//bool black = !node.m_black;

			const int s = pNode->m_children.size();
			std::vector<std::shared_ptr<TestNode>> nodes;
			for (int i {0}; i < s; ++i) {
				nodes.push_back(pNode->m_children[i]);
			}

			sortNodes(nodes);

			if (verbose) std::cout  << "s=" << s << std::flush;


			for (int i {0}; i < s; ++i) {
				//if (!env.m_discarded) {
					std::shared_ptr<TestNode>& pChild = nodes[i];//node.m_children[i];
					std::shared_ptr<Context<TestNode>> pChildEnv = std::make_shared<Context<TestNode>>(pChild, pCtxt, pChild->m_children.size());

					//node.addChild(std::make_shared<TestNode>(child));
					m_totalNodeCount++;

					buildFrom(pChildEnv);
					if (ctxt.m_closed) {
						break;
					}

				//}
			}
		} else {
			ctxt.m_value = pNode->m_value;
			updateAll(ctxt);
		}
		//std::cout << "\nbuildFrom " << *node << " done" << std::flush;
		} catch (...) {
			std::cout << "Exception caught." << std::flush;
		}
	}

};

} /* namespace othello */

