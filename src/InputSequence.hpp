/*
 * InputSequence.h
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_INPUTSEQUENCE_HPP_
#define SRC_INPUTSEQUENCE_HPP_

#include <optional>
#include "Token.hpp"

namespace dirac {

class InputSequence {
public:
	virtual std::optional<Token> nextToken() = 0;
	virtual ~InputSequence() = default;
};

}


#endif /* SRC_INPUTSEQUENCE_HPP_ */
