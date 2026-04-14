/*
 * EvalStrategy.h
 *
 *  Created on: Jun 10, 2024
 *      Author: sandro
 */

#pragma once

#include "../../tree/TreeVisitor.h"

namespace othello {

template<typename T>
class Tree;

class Node;

class EvalStrategy : public TreeVisitor<Node> {
public:
	EvalStrategy(US maxDepth, short borderWeight = BORDER_WEIGHT, short mobilityWeight = MOBILITY_WEIGHT) :
		m_maxDepth(maxDepth),
		m_borderWeight(borderWeight),
		m_mobilityWeight(mobilityWeight) {
	};
	virtual ~EvalStrategy() = default;
	EvalStrategy(const EvalStrategy &other) = default;
	EvalStrategy(EvalStrategy &&other) = default;
	EvalStrategy& operator=(const EvalStrategy &other) = default;
	EvalStrategy& operator=(EvalStrategy &&other) = default;

	virtual void visit(Tree<Node>& tree) override;

	virtual void visit(Node& node) override;

	void eval(Node& node) const;

	US m_maxDepth;
	std::vector<std::pair<US, short>> m_scores;

	std::pair<US, short> m_result;

	short m_borderWeight;
	short m_mobilityWeight;

	static constexpr short BORDER_WEIGHT = 5;
	static constexpr short MOBILITY_WEIGHT = 10;
	static constexpr short V[4][4] = {{11,12,21,22},{18,17,27,28},{81,72,71,82},{88,77,78,87}}; //Top-left, bottom-left,... borders


};

} /* namespace othello */

