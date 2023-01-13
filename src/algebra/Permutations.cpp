/*
 * Permutation.cpp
 *
 *  Created on: Jan 4, 2023
 *      Author: skutnii
 */

#include "Permutations.hpp"

namespace dirac {

namespace algebra {

void forPermutations(unsigned int n, PermutationWalker walker) {
	if (n == 0) {
		walker(Permutation{});
		return;
	}

	if (n == 1) {
		Permutation triv;
		triv.map.push_back(0);
		walker(triv);
		return;
	}

	for (unsigned int k = 0; k < n; ++k)
		forPermutations(n - 1,
			[k, n, &walker](const Permutation& rest) {
				Permutation combo;
				combo.map.reserve(n);
				combo.map.push_back(k);
				for (unsigned int l : rest.map)
					if (l < k)
						combo.map.push_back(l);
					else
						combo.map.push_back(l + 1);

				bool kEven = (k % 2) == 0;
				combo.isEven = (rest.isEven && kEven)
										|| (!rest.isEven && !kEven);
				walker(combo);
			});
}


}

}


