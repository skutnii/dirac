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
	res._value(2, 1) = -(I() / 2.0) * (eta(mu, lambda1) * eta(nu, lambda2) -
			eta(mu, lambda2) * eta(nu, lambda1));
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
PseudoMatrix::sigma(const TensorIndex& mu1, const TensorIndex &mu2,
		int leftTag, int rightTag) {
	TensorIndex nu{ IndexTag{ leftTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ leftTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ leftTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ rightTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ rightTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ rightTag, 2 }, false };

	PseudoMatrix res{ leftTag, rightTag };
	res._value(0, 2) = eta(mu1, nu1) * eta(mu2, nu2) - eta(mu2, nu1) * eta(mu1, nu2);
	res._value(1, 1) = I() * (eta(mu1, lambda) * eta(mu2, nu)
			- eta(mu2, lambda) * eta(mu1, nu));
	res._value(1, 3) = epsilon(mu1, mu2, nu, lambda);
	res._value(2, 0) = (1.0 / 2.0) *
			(eta(mu1, lambda1) * eta(mu2, lambda2)
					- eta(mu2, lambda1) * eta(mu1, lambda2));
	res._value(2, 4) = - (I() / 2.0) * epsilon(mu1, mu2, lambda1, lambda2);
	res._value(3, 1) = (1.0 / 2.0) * epsilon(mu1, mu2, nu, lambda);
	res._value(3, 3) = (I() / 2.0)
			* (eta(mu1, nu) * eta(mu2, lambda)
					- eta(mu2, nu) * eta(mu1, lambda));
	res._value(4, 2) = (I() / 2.0) * epsilon(mu1, mu2, nu1, nu2);

	return res;
}

PseudoMatrix::PseudoMatrix(const LI::TensorPolynomial& tp) {
	_value(0, 0) = tp;
	_value(1, 1) = tp;
	_value(2, 2) = tp;
	_value(3, 3) = tp;
	_value(4, 4) = tp;
}

PseudoMatrix::PseudoMatrix(const Complex &c) {
	_value(0, 0) = c;
	_value(1, 1) = c;
	_value(2, 2) = c;
	_value(3, 3) = c;
	_value(4, 4) = c;
}

} /* namespace algebra */

} /* namespace dirac */
