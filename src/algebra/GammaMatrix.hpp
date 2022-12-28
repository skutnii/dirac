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

using GammaMatrix = Eigen::Matrix<LI::TensorPolynomial, 5, 5>;
using GammaVector = Eigen::Matrix<LI::TensorPolynomial, 5, 1>;

GammaMatrix gamma(const TensorIndex& idx, int leftTag, int rightTag);
GammaMatrix gamma5(int leftTag, int rightTag);
GammaMatrix sigma(const TensorIndex& idx1,
		const TensorIndex& idx2,
		int leftTag,
		int rightTag);

inline GammaMatrix diag(const LI::TensorPolynomial& coeff) {
	GammaMatrix res;
	for (unsigned int i = 0; i < 5; ++i)
		res(i, i) = coeff;

	return res;
}

} /* namespace algebra */

} /* namespace dirac */

#endif /* SRC_PSEUDOMATRIX_H_ */
