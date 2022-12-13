/*
 * algebra.h
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_ALGEBRA_H_
#define SRC_ALGEBRA_H_

#include <vector>
#include <complex>
#include <memory>
#include <algorithm>

#include "Tensor.h"

namespace dirac {

namespace algebra {

struct Monomial {
	std::complex<double> coeff{ 1, 0 };
	std::vector<Tensor> factors;

	Monomial() = default;
	Monomial(const Monomial& other) = default;
	Monomial(Monomial&& other) = default;

	Monomial& operator=(const Monomial& other) = default;

	Monomial(std::complex<double> z) : coeff{z} {}
	Monomial(const Tensor& factor) {
		factors.push_back(factor);
	}

	//Product constructor
	Monomial(const Monomial& first, const Monomial& second);

	Monomial operator*(const Monomial& other) const {
		return Monomial(*this, other);
	}

	Monomial operator*(std::complex<double> coeff) const {
		Monomial clone{ *this };
		clone.coeff *= coeff;
		return clone;
	}
};

using MonomialPtr = std::shared_ptr<Monomial>;
using Polynomial = std::vector<MonomialPtr>;

inline Monomial operator*(std::complex<double> coeff, const Monomial& m) {
	return m * coeff;
}

inline MonomialPtr operator*(std::complex<double> coeff, const MonomialPtr& m) {
	MonomialPtr clone = std::make_shared<Monomial>(*m);
	clone->coeff *= coeff;
	return clone;
}

inline MonomialPtr operator*(const MonomialPtr& m, std::complex<double> coeff) {
	return coeff * m;
}

inline MonomialPtr operator*(const MonomialPtr& first, const MonomialPtr& second) {
	return std::make_shared<Monomial>(*first, *second);
}

inline Polynomial operator+(const MonomialPtr& first, const MonomialPtr& second) {
	return Polynomial{ first, second };
}

Polynomial operator*(std::complex<double> coeff, const Polynomial& p);

Polynomial operator+(const Polynomial& first, const Polynomial& second);

Polynomial operator*(const Polynomial& first, const Polynomial& second);

}

}

#endif /* SRC_ALGEBRA_H_ */
