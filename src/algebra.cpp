/*
 * algebra.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#include "algebra.h"
#include <stdexcept>

namespace dirac {

namespace algebra {

Polynomial operator*(const Polynomial& first, const Polynomial& second) {
	Polynomial prod;
	size_t size = first.size() * second.size();
	prod.reserve(size);

	if (size == 0)
		return prod;

	for (const MonomialPtr& m1: first)
		for (const MonomialPtr& m2: second)
			prod.push_back(std::make_shared<Monomial>(*m1, *m2));

	return prod;
}

Polynomial operator*(std::complex<double> coeff, const Polynomial& p) {
	Polynomial result;
	result.resize(p.size());
	std::transform(p.begin(), p.end(), result.begin(),
			[coeff](const MonomialPtr& m) {
				return coeff * m;
			});

	return result;
}

Polynomial operator+(const Polynomial& first, const Polynomial& second) {
	Polynomial sum;
	sum.reserve(first.size() + second.size());
	sum.insert(sum.end(), first.begin(), first.end());
	sum.insert(sum.end(), second.begin(), second.end());

	return sum;
}

Monomial::Monomial(const Monomial &first, const Monomial &second) :
	coeff{first.coeff * second.coeff} {
	factors.insert(factors.end(), first.factors.begin(), first.factors.end());
	factors.insert(factors.end(), second.factors.begin(), second.factors.end());
}

} //Namespace algebra

} //Namespace dirac
