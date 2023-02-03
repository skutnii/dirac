/*
 * Identity.hpp
 *
 *  Created on: Jan 28, 2023
 *      Author: skutnii
 */

#ifndef EXAMPLES_FIERZ_GEN_IDENTITY_HPP_
#define EXAMPLES_FIERZ_GEN_IDENTITY_HPP_

#include "Expression.hpp"

namespace fierz {

struct Identity {
	Expression left;
	std::vector<std::pair<std::string, std::string>> leftSpinorIndices;

	Expression right;
	std::vector<std::pair<std::string, std::string>> rightSpinorIndices;
};

} /* namespace fierz */

#endif /* EXAMPLES_FIERZ_GEN_IDENTITY_HPP_ */
