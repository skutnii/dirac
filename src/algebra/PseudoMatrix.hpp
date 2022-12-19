/*
 * PseudoMatrix.h
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_PSEUDOMATRIX_H_
#define SRC_PSEUDOMATRIX_H_

#include <Eigen>
#include "LorentzInvariant.hpp"
#include <utility>
#include <optional>

namespace Eigen {

template<>
struct NumTraits<dirac::algebra::LI::TensorPolynomial>
{
  typedef dirac::algebra::LI::TensorPolynomial Real;
  typedef dirac::algebra::LI::TensorPolynomial NonInteger;
  typedef dirac::algebra::LI::TensorPolynomial Nested;
  typedef std::complex<double> Literal;

  enum {
    IsComplex = 0,
    IsInteger = 0,
    IsSigned = 1,
    RequireInitialization = 1,
    ReadCost = HugeCost,
    AddCost = HugeCost,
    MulCost = HugeCost
  };
};

} /* namespace Eigen*/

namespace dirac {

namespace algebra {

class PseudoMatrix {
public:
	using Element = LI::TensorPolynomial;
	using Value = Eigen::Matrix<Element, 5, 5>;

	PseudoMatrix(const PseudoMatrix& other) = default;
	PseudoMatrix(PseudoMatrix&& other) = default;
	PseudoMatrix& operator=(const PseudoMatrix& other) = default;

	const Value& value() { return _value; }

	int leftTag() const {
		return _leftTag;
	}

	int rightTag() const {
		return _rightTag;
	}

	bool indicesEqual(const PseudoMatrix& other) const;

	/**
	 * Multiply two pseudomatrices by value.
	 * Requires *this.rightTag() == other.leftTag(),
	 * otherwise an error is thrown.
	 */
	PseudoMatrix operator*(const PseudoMatrix& other) const;

	PseudoMatrix operator+(const PseudoMatrix& other) const;

	PseudoMatrix operator-(const PseudoMatrix& other) const;

	static PseudoMatrix gamma(const TensorIndex& idx, int leftTag, int rightTag);
	static PseudoMatrix gamma5(int leftTag, int rightTag);
	static PseudoMatrix sigma(const TensorIndex& idx1,
			const TensorIndex& idx2,
			int leftTag,
			int rightTag);

private:

	PseudoMatrix() = default;

	PseudoMatrix(int leftTag, int rightTag, const Value& value = Value{})
	: _leftTag{leftTag}, _rightTag{rightTag}, _value{value} {}

	int _leftTag = 1;
	int _rightTag = 2;
	Value _value;
};

} /* namespace algebra */

} /* namespace dirac */

#endif /* SRC_PSEUDOMATRIX_H_ */
