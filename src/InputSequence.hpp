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

template<class T, typename Number>
concept InputSequence = requires(T a) {
	{ a.nextToken() } -> std::same_as<std::optional<Token<Number>>>;
};

}


#endif /* SRC_INPUTSEQUENCE_HPP_ */
