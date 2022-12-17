/*
 * Polynomials.hpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_POLYNOMIALRING_HPP_
#define SRC_POLYNOMIALRING_HPP_

#include "concepts.hpp"
#include <memory>
#include <vector>
#include <concepts>
#include "concepts.hpp"
#include <algorithm>

namespace dirac {

namespace algebra {

template<typename T>
concept ScalarType = requires(T a, T b) {
	a + b;
	a - b;
	a * b;
	-a;
};

template<ScalarType Coeff, typename Factor>
struct Polynomial {
	struct Term {
		Coeff coeff;
		std::vector<Factor> factors;

		Term operator*(const Term& other) const {
			Term prod;
			prod.coeff = coeff * other.coeff;
			prod.factors.insert(prod.factors.end(), factors.begin(), factors.end());
			prod.factors.insert(prod.factors.end(),
					other.factors.begin(), other.factors.end());
			return prod;
		}

		Term operator-() const {
			Term neg;
			neg.coeff = -coeff;
			neg.factors = factors;
			return neg;
		}

		Term() = default;
		Term(const Term& other) = default;
		Term(Term&& other) = default;

		Term& operator=(const Term& other) = default;

		Term(const Coeff& c) : coeff{ c } {}
	};

	using Terms = std::vector<Term>;
	Terms terms;

	Polynomial() = default;
	Polynomial(const Polynomial<Coeff, Factor>& other) = default;
	Polynomial(Polynomial<Coeff, Factor>&& other) = default;

	Polynomial<Coeff, Factor>&
	operator=(const Polynomial<Coeff, Factor>& other) = default;
};

/**
 * Polynomial normalization method.
 * Default implementation does nothing.
 * Specialize for specific polynomial types.
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
inline void canonicalize(P& p) {
}

/**
 * Polynomial addition
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P sum(const P& p1, const P& p2) {
	P sum;

	sum.terms.insert(sum.terms.end(), p1.terms.begin(), p1.terms.end());
	sum.terms.insert(sum.terms.end(), p2.terms.begin(), p2.terms.end());
	canonicalize<P, CoeffType, Factor>(sum);
	return sum;
}

/**
 * Polynomial subtraction
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P diff(const P& p1, const P& p2) {
	P diff;

	using Term = Polynomial<CoeffType, Factor>::Term;

	diff.terms.insert(diff.terms.end(), p1.terms.begin(), p1.terms.end());
	diff.terms.resize(p1.terms.size() + p2.terms.size());
	std::transform(p2.terms.begin(), p2.terms.end(),
			diff.terms.begin() + p1.terms.size(),
			[](const Term& t) {
				return -t;
			});

	canonicalize<P, CoeffType, Factor>(diff);
	return diff;
}

/**
 * Polynomial multiplication
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P prod(const P& p1, const P& p2) {
	P prod;
	size_t size1 = p1.terms.size();
	size_t size2 = p2.terms.size();
	size_t total = size1 * size2;
	if (total == 0)
		return prod;

	prod.terms.reserve(total);
	for (size_t i = 0; i < size1; ++i)
		for (size_t j = 0; j < size2; ++j)
			prod.terms.emplace_back(p1.terms[i] * p2.terms[j]);

	canonicalize<P, CoeffType, Factor>(prod);

	return prod;
}

}

}



#endif /* SRC_POLYNOMIALRING_HPP_ */
