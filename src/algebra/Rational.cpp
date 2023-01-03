/*
 * Rational.cpp
 *
 *  Created on: Dec 28, 2022
 *      Author: skutnii
 */

#include "Rational.hpp"
#include <unordered_set>
#include <list>
#include <cmath>

namespace dirac {

namespace algebra {

void Rational::normalize() {
	if ((_num == 0) || (_den == 0))
		return;

	removeFactor(2);

	for (unsigned long long int div = 3;
			div < std::sqrt(std::min(absNum(), _den)); div += 2)
		removeFactor(div);
}

void Rational::removeFactor(unsigned long long int f) {
	while (((absNum() % f) == 0) && ((_den % f) == 0)) {
		_num = _num / f;
		_den = _den / f;
	}
}

}

}
