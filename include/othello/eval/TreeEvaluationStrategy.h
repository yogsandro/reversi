/*
 * TreeEvaluationStrategy.h
 *
 *  Created on: Jun 14, 2024
 *      Author: sandro
 */

#pragma once
#include "tree/Tree.h"

namespace othello {

template<typename T>
class TreeEvaluationStrategy {
public:
	TreeEvaluationStrategy() = default;
	virtual ~TreeEvaluationStrategy() = default;
	TreeEvaluationStrategy(const TreeEvaluationStrategy &other) = default;
	TreeEvaluationStrategy(TreeEvaluationStrategy &&other) = default;
	TreeEvaluationStrategy& operator=(const TreeEvaluationStrategy &other) = default;
	TreeEvaluationStrategy& operator=(TreeEvaluationStrategy &&other) = default;

	virtual void eval(Tree<T>& tree) = 0;
	virtual void eval(T& node) = 0;
	virtual void doEval(T& node) = 0;

};

} /* namespace othello */

