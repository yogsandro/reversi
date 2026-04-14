/*
 * SEvaluationStrategy.h
 *
 *  Created on: Jun 28, 2024
 *      Author: sandro
 */

#pragma once

#include "EvaluationStrategy.h"
#include "othello/OthelloTree.h"

namespace othello {

class SEvaluationStrategy : public EvaluationStrategy<Node>
 {
public:
	SEvaluationStrategy()= default;
	virtual ~SEvaluationStrategy() = default;
	SEvaluationStrategy(const SEvaluationStrategy &other) = default;
	SEvaluationStrategy(SEvaluationStrategy &&other) = default;
	SEvaluationStrategy& operator=(const SEvaluationStrategy &other) = default;
	SEvaluationStrategy& operator=(SEvaluationStrategy &&other) = default;

	virtual void eval(Node& node) override;

};

} /* namespace othello */

