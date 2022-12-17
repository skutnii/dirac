/*
 * PseudoMatrix.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#include "PseudoMatrix.hpp"
#include <stdexcept>
#include "IndexId.hpp"

namespace dirac {

namespace algebra {

PseudoMatrix PseudoMatrix::operator*(const PseudoMatrix& other) const {
	if (_rightTag != other._leftTag)
		throw std::runtime_error{
			"Can only multiply pseudomatrices with matching tags" };

	return PseudoMatrix{ _leftTag, other._rightTag, _value * other._value };
}

bool PseudoMatrix::indicesEqual(const PseudoMatrix &other) const {
	return (_leftTag == other._leftTag)
			&& (_rightTag == other._rightTag);
}

PseudoMatrix PseudoMatrix::operator+(const PseudoMatrix& other) const {
	if (!indicesEqual(other))
		throw std::runtime_error{ "Can only add two pseudomatrices with same indices" };

	return PseudoMatrix{ _leftTag, _rightTag, _value + other._value };
}

PseudoMatrix PseudoMatrix::operator-(const PseudoMatrix& other) const {
	if (!indicesEqual(other))
		throw std::runtime_error{ "Can only subtract two pseudomatrices with same indices" };

	return PseudoMatrix{ _leftTag, _rightTag, _value - other._value };
}

using namespace LI;

PseudoMatrix
PseudoMatrix::gamma(const TensorIndex& mu, int leftTag, int rightTag) {
	TensorIndex nu{ IndexTag{ leftTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ leftTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ leftTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ rightTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ rightTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ rightTag, 2 }, false };

	PseudoMatrix res{ leftTag, rightTag };
	res._value(0, 1) = eta(mu, nu);
	res._value(1, 0) = eta(mu, lambda);
	res._value(1, 2) = I() * (eta(mu, nu1) * eta(nu2, lambda)
								- eta(mu, nu2) * eta(nu1, lambda));
	res._value(2, 1) = -I() * eta(mu, lambda1) * eta(nu, lambda2);
	res._value(2, 3) = (one() / 2.0) * epsilon(mu, nu, lambda1, lambda2);
	res._value(3, 2) = epsilon(mu, nu1, nu2, lambda);
	res._value(3, 4) = -eta(mu, lambda);
	res._value(4, 3) = -eta(mu, nu);

	return res;
}

PseudoMatrix PseudoMatrix::gamma5(int leftTag, int rightTag) {
	TensorIndex nu{ IndexTag{ leftTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ leftTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ leftTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ rightTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ rightTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ rightTag, 2 }, false };

	PseudoMatrix res{ leftTag, rightTag };
	res._value(0, 4) = one();
	res._value(1, 3) = eta(nu, lambda);
	res._value(2, 2) = - (I() / 2.0) * epsilon(nu1, nu2, lambda1, lambda2);
	res._value(3, 1) = eta(nu, lambda);
	res._value(4, 0) = one();

	return res;
}

PseudoMatrix
PseudoMatrix::sigma(const TensorIndex& mu, const TensorIndex &nu,
		int leftTag, int rightTag) {
	PseudoMatrix res{ leftTag, rightTag };
	return res;
}

} /* namespace algebra */

} /* namespace dirac */
