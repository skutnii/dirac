/*
 * Permutations.hpp
 *
 *  Created on: Jan 4, 2023
 *      Author: skutnii
 */

#ifndef SRC_ALGEBRA_PERMUTATIONS_HPP_
#define SRC_ALGEBRA_PERMUTATIONS_HPP_

#include <functional>
#include <vector>

namespace dirac {

namespace algebra {

struct Permutation {
	std::vector<unsigned int> map;
	bool isEven = true;
};

using PermutationWalker = std::function<void (const Permutation&)>;

void forPermutations(unsigned int n, PermutationWalker walker);

}

}



#endif /* SRC_ALGEBRA_PERMUTATIONS_HPP_ */
