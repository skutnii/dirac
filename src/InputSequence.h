/*
 * InputSequence.h
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_INPUTSEQUENCE_H_
#define SRC_INPUTSEQUENCE_H_

#include "Token.h"
#include <optional>

namespace dirac {

class InputSequence {
public:
	virtual std::optional<Token> nextToken() = 0;
	virtual ~InputSequence() = default;
};

}


#endif /* SRC_INPUTSEQUENCE_H_ */
