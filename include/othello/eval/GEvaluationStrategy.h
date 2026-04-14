/*
 * GEvaluationStrategy.h
 *
 *  Created on: Jun 14, 2024
 *      Author: sandro
 */

#pragma once

#include "TreeEvaluationStrategy.h"
#include "othello/OthelloTree.h"

namespace othello {

class GEvaluationStrategy : public TreeEvaluationStrategy<Node>
 {
public:
	GEvaluationStrategy(unsigned short maxDepth, short borderWeight = BORDER_WEIGHT, short mobilityWeight = MOBILITY_WEIGHT) :
		TreeEvaluationStrategy(),
		m_maxDepth(maxDepth),
		m_borderWeight(borderWeight),
		m_mobilityWeight(mobilityWeight) {
	}

	virtual ~GEvaluationStrategy() = default;
	GEvaluationStrategy(const GEvaluationStrategy &other) = default;
	GEvaluationStrategy(GEvaluationStrategy &&other) = default;
	GEvaluationStrategy& operator=(const GEvaluationStrategy &other) = default;
	GEvaluationStrategy& operator=(GEvaluationStrategy &&other) = default;

	virtual void eval(Tree<Node>& tree) override;
	virtual void eval(Node& node) override;
	virtual void doEval(Node& node) override ;

	unsigned short maxDepth() const {
		return m_maxDepth;
	}

	const std::pair<unsigned short, short>& getResult() const {
		return m_result;
	}

private:
	unsigned short m_maxDepth;
	std::vector<std::pair<unsigned short, short>> m_scores;

	std::pair<unsigned short, short> m_result;

	short m_borderWeight;
	short m_mobilityWeight;

	static constexpr short BORDER_WEIGHT = 5;
	static constexpr short MOBILITY_WEIGHT = 10;
	static constexpr short V[4][4] = {{11,12,21,22},{18,17,27,28},{81,72,71,82},{88,77,78,87}}; //Top-left, bottom-left,... borders
	static constexpr UL VL[4][4] = {
			{1L<<48 | Board::HIGH_TYPE, 1L<<38 | Board::HIGH_TYPE, 1L<<47 | Board::HIGH_TYPE,1L<<37 | Board::HIGH_TYPE},
			{1L<<18, 1L<<28, 1L<<27, 1L<<17},
			{1L<<41 | Board::HIGH_TYPE, 1L<<32 | Board::HIGH_TYPE, 1L<<42 | Board::HIGH_TYPE,1L<<31 | Board::HIGH_TYPE},
			{1L<<11, 1L<<22, 1L<<12, 1L<<21}
	}; //Top-left, bottom-left,... borders

};

} /* namespace othello */

