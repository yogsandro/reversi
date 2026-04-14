/*
 * EvaluationStrategy.h
 *
 *  Created on: Jun 30, 2024
 *      Author: sandro
 */

#pragma once
#include <memory>

#include "othello/GameRegistry.h"

namespace othello {

template<typename T>
class EvaluationStrategy {
public:
	EvaluationStrategy() = default;
	virtual ~EvaluationStrategy() = default;
	EvaluationStrategy(const EvaluationStrategy &other) = default;
	EvaluationStrategy(EvaluationStrategy &&other) = default;
	EvaluationStrategy& operator=(const EvaluationStrategy &other) = default;
	EvaluationStrategy& operator=(EvaluationStrategy &&other) = default;

	virtual void eval(T& node) = 0;

	virtual void setGame(std::shared_ptr<GameRegistry> pGame) {
		m_pGame = pGame;
	}

protected:
	std::shared_ptr<GameRegistry> m_pGame;
};

} /* namespace othello */

