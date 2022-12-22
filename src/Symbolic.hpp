/*
 * Symbolic.hpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */

#ifndef SRC_SYMBOLIC_HPP_
#define SRC_SYMBOLIC_HPP_

#include "algebra/TensorBase.hpp"
#include "algebra/LorentzInvariant.hpp"
#include <string>
#include <unordered_set>
#include "algebra/Polynomials.hpp"
#include <ostream>

namespace dirac {

namespace symbolic {

struct Basis {
	Basis() = default;

	static const std::string gamma;
	static const std::string sigma;
	static const std::string gamma5;

	using NameSet = std::unordered_set<std::string>;
	static const NameSet Elements;

	inline static bool allows(const std::string& id) {
		return (algebra::LI::Basis::allows(id)
				|| (Elements.find(id) != Elements.end()));
	}

	inline static size_t maxIndexCount(const std::string& id) {
		if (algebra::LI::Basis::allows(id))
			return algebra::LI::Basis::maxIndexCount(id);

		if (id == sigma)
			return 2;

		if (id == gamma)
			return 1;

		return 0;
	}
};

using Tensor = algebra::TensorBase<std::string, Basis, algebra::IndexId>;
using Complex = algebra::Complex;
using GammaPolynomial = algebra::Polynomial<Complex, Tensor>;

using TensorIndex = algebra::TensorIndex;
using TensorIndices = algebra::TensorIndices;

GammaPolynomial toPolynomial(const Tensor& t);

GammaPolynomial reduceGamma(const GammaPolynomial& p);

}

inline symbolic::GammaPolynomial
operator+(const symbolic::GammaPolynomial& p1,
		const symbolic::GammaPolynomial& p2) {
	return algebra::sum<symbolic::GammaPolynomial, algebra::Complex, symbolic::Tensor>(p1, p2);
}

inline symbolic::GammaPolynomial
operator-(const symbolic::GammaPolynomial& p1,
		const symbolic::GammaPolynomial& p2) {
	return algebra::diff<symbolic::GammaPolynomial, algebra::Complex, symbolic::Tensor>(p1, p2);
}

inline symbolic::GammaPolynomial
operator-(const symbolic::GammaPolynomial& p) {
	return algebra::negate<symbolic::GammaPolynomial, algebra::Complex, symbolic::Tensor>(p);
}

inline symbolic::GammaPolynomial
operator*(const symbolic::GammaPolynomial& p1,
		const symbolic::GammaPolynomial& p2) {
	return algebra::prod<symbolic::GammaPolynomial, algebra::Complex, symbolic::Tensor>(p1, p2);
}

inline symbolic::GammaPolynomial
operator*(const algebra::Complex& c,
		const symbolic::GammaPolynomial& p) {
	return algebra::prod<symbolic::GammaPolynomial, algebra::Complex, symbolic::Tensor>(c, p);
}

inline symbolic::GammaPolynomial
operator*(const symbolic::GammaPolynomial& p,
		const algebra::Complex& c) {
	return algebra::prod<symbolic::GammaPolynomial, algebra::Complex, symbolic::Tensor>(p, c);
}

std::ostream& operator<<(std::ostream& os, const symbolic::GammaPolynomial& poly);

}



#endif /* SRC_SYMBOLIC_HPP_ */
