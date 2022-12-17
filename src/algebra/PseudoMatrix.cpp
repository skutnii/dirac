/*
 * PseudoMatrix.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#include "PseudoMatrix.h"
#include <stdexcept>

namespace dirac {

namespace algebra {

PseudoMatrix PseudoMatrix::operator*(const PseudoMatrix& other) const {
	if (_rightTag != other._leftTag)
		throw std::runtime_error{
			"Can only multiply pseudomatrices with matching tags" };

	PseudoMatrix prod{ _value * other._value };
	prod._leftTag = _leftTag;
	prod._rightTag = other._rightTag;

	return prod;
}

bool PseudoMatrix::indicesEqual(const PseudoMatrix &other) const {
	return (_leftTag == other._leftTag)
			&& (_rightTag == other._rightTag);
}

PseudoMatrix PseudoMatrix::operator+(const PseudoMatrix& other) const {
	if (!indicesEqual(other))
		throw std::runtime_error{ "Can only add two pseudomatrices with same indices" };

	PseudoMatrix result{ _value + other._value };
	result._leftTag = _leftTag;
	result._rightTag = _rightTag;

	return result;
}

PseudoMatrix PseudoMatrix::operator-(const PseudoMatrix& other) const {
	if (!indicesEqual(other))
		throw std::runtime_error{ "Can only subtract two pseudomatrices with same indices" };

	PseudoMatrix result{ _value - other._value };
	result._leftTag = _leftTag;
	result._rightTag = _rightTag;

	return result;
}


} /* namespace algebra */

} /* namespace dirac */
