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

/**
 * n-element permutation representation.
 */
struct Permutation {
	/**
	 * Must be an n-element vector
	 * containing all integers from 0 to n-1
	 * in arbitrary order.
	 */
	std::vector<unsigned int> map;

	/**
	 * Specifies whether the permutation is even,
	 * that is, can be represented as a sequence of even number
	 * of elementary permutations.
	 */
	bool isEven = true;
};

using PermutationWalker = std::function<void (const Permutation&)>;

/**
 * Recursively enumerates all permutations of n elements
 */
void forPermutations(unsigned int n, PermutationWalker walker);

}

}



#endif /* SRC_ALGEBRA_PERMUTATIONS_HPP_ */
