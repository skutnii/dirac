/*
 * Polynomials.hpp
 *
 * Abstract polynomial algebra.
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_POLYNOMIALS_HPP_
#define SRC_POLYNOMIALS_HPP_

#include "concepts.hpp"
#include <memory>
#include <vector>
#include <concepts>
#include "concepts.hpp"
#include <algorithm>

namespace dirac {

namespace algebra {

/**
 * Requirements for a polynomial coefficient
 */
template<typename T>
concept ScalarType = requires(T a, T b) {
	a + b;
	a - b;
	a * b;
	-a;
};

/**
 * Abstract polynomial type
 */
template<ScalarType Coeff, typename Factor>
struct Polynomial {
	/**
	 * Polynomial term structure
	 */
	struct Term {
		Coeff coeff;
		std::vector<Factor> factors;

		/**
		 * Terms multiplication
		 */
		Term operator*(const Term& other) const {
			Term prod;
			prod.coeff = coeff * other.coeff;
			prod.factors.insert(prod.factors.end(),
						factors.begin(), factors.end());
			prod.factors.insert(prod.factors.end(),
					other.factors.begin(), other.factors.end());
			return prod;
		}

		/**
		 * Term negation
		 */
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

		Term(const Factor& f) : Term{} {
			factors.push_back(f);
		}

		Term(const Coeff& c, const Factor& f) :
						coeff{ c }, factors{ f } {}
	};

	using Terms = std::vector<Term>;
	Terms terms;

	Polynomial() = default;
	Polynomial(const Polynomial<Coeff, Factor>& other) = default;
	Polynomial(Polynomial<Coeff, Factor>&& other) = default;

	Polynomial<Coeff, Factor>&
	operator=(const Polynomial<Coeff, Factor>& other) = default;
	virtual ~Polynomial() = default;

	Polynomial(const Coeff& c) {
		terms.emplace_back(c);
	}

	/**
	 * Polynomial canonicalization method.
	 * Default implementation does nothing.
	 * Override for specific polynomial types.
	 */
	virtual void canonicalize() {
	}

};

//----------------------------------------------------------------------

/**
 * Mutating polynomial addition ("+=" operator)
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P& add(P& p1, const P& p2) {
	p1.terms.insert(p1.terms.end(), p2.terms.begin(), p2.terms.end());
	p1.canonicalize();
	return p1;
}

//----------------------------------------------------------------------

/**
 * Polynomial addition ("+" operator)
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P sum(const P& p1, const P& p2) {
	P sum{ p1 };
	return add<P, CoeffType, Factor>(sum, p2);
}

//----------------------------------------------------------------------

/**
 * Mutating polynomial subtraction ("-=" operator)
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P& sub(P& p1, const P& p2) {
	using Term = Polynomial<CoeffType, Factor>::Term;

	size_t firstTermCount = p1.terms.size();
	p1.terms.resize(p1.terms.size() + p2.terms.size());
	std::transform(p2.terms.begin(), p2.terms.end(),
			p1.terms.begin() + firstTermCount,
			[](const Term& t) {
				return -t;
			});

	p1.canonicalize();
	return p1;
}

//----------------------------------------------------------------------

/**
 * Polynomial subtraction
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P diff(const P& p1, const P& p2) {
	P diff{ p1 };
	return sub<P, CoeffType, Factor>(diff, p2);
}

//----------------------------------------------------------------------

/**
 * Polynomial negation
 */
template<class P, typename CoeffType, typename Factor>
requires std::derived_from<P, Polynomial<CoeffType, Factor> >
P negate(const P& p) {
	P res;

	using Term = Polynomial<CoeffType, Factor>::Term;

	res.terms.resize(p.terms.size());
	std::transform(p.terms.begin(), p.terms.end(),
			res.terms.begin(),
			[](const Term& t) {
				return -t;
			});

	return res;
}

//----------------------------------------------------------------------

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

	prod.canonicalize();

	return prod;
}

//----------------------------------------------------------------------

/**
 * Left multiplication of a polynomial by number
 */
template<class P, typename Coeff, typename Factor>
requires std::derived_from<P, Polynomial<Coeff, Factor> >
P prod(const Coeff& c, const P& p) {
	P prod;
	if (p.terms.empty())
		return prod;

	using Term = Polynomial<Coeff, Factor>::Term;
	prod.terms.resize(p.terms.size());
	std::transform(p.terms.begin(), p.terms.end(),
			prod.terms.begin(),
			[&c](const Term& t) {
				Term res{ t };
				res.coeff = c * t.coeff;
				return res;
			});

	return prod;
}

//----------------------------------------------------------------------

/**
 * Right multiplication of a polynomial by number.
 * Note that in the most general case numbers may be non-commutative.
 */
template<class P, typename Coeff, typename Factor>
requires std::derived_from<P, Polynomial<Coeff, Factor> >
P prod(const P& p, const Coeff& c) {
	P prod;
	if (p.terms.empty())
		return prod;

	using Term = Polynomial<Coeff, Factor>::Term;
	prod.terms.resize(p.terms.size());
	std::transform(p.terms.begin(), p.terms.end(),
			prod.terms.begin(),
			[&c](const Term& t) {
				Term res{ t };
				res.coeff = t.coeff * c;
				return res;
			});

	return prod;
}

} /*namespace algebra*/

} /*namespce dirac*/

#endif /* SRC_POLYNOMIALS_HPP_ */
