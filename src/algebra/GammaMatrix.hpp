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

template<typename Scalar>
struct NumTraits<dirac::algebra::LI::TensorPolynomial<Scalar> >
{
  typedef dirac::algebra::LI::TensorPolynomial<Scalar> Real;
  typedef dirac::algebra::LI::TensorPolynomial<Scalar> NonInteger;
  typedef dirac::algebra::LI::TensorPolynomial<Scalar> Nested;
  typedef dirac::algebra::Complex<Scalar> Literal;

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

template<typename Scalar>
using GammaMatrix = Eigen::Matrix<LI::TensorPolynomial<Scalar>, 5, 5>;

template<typename Scalar>
using GammaVector = Eigen::Matrix<LI::TensorPolynomial<Scalar>, 5, 1>;


template<typename Scalar>
GammaMatrix<Scalar> gamma(const TensorIndex& mu, int leftTag, int rightTag)  {
	TensorIndex nu{ IndexTag{ rightTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ rightTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ rightTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ leftTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ leftTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ leftTag, 2 }, false };

	using namespace LI;

	GammaMatrix<Scalar> res;
	res(0, 1) = eta<Scalar>(mu, nu);
	res(1, 0) = eta<Scalar>(mu, lambda);
	res(1, 2) = I<Scalar>() * (eta<Scalar>(mu, nu1) * eta<Scalar>(nu2, lambda)
								- eta<Scalar>(mu, nu2) * eta<Scalar>(nu1, lambda));
	res(2, 1) = -(I<Scalar>() / Scalar{ 2 }) * (eta<Scalar>(mu, lambda1) * eta<Scalar>(nu, lambda2) -
			eta<Scalar>(mu, lambda2) * eta<Scalar>(nu, lambda1));
	res(2, 3) = (one<Scalar>() / Scalar{ 2 }) * epsilon<Scalar>(mu, nu, lambda1, lambda2);
	res(3, 2) = -epsilon<Scalar>(mu, nu1, nu2, lambda);
	res(3, 4) = -eta<Scalar>(mu, lambda);
	res(4, 3) = -eta<Scalar>(mu, nu);

	return res;
}

template<typename Scalar>
GammaMatrix<Scalar> gamma5(int leftTag, int rightTag) {
	TensorIndex nu{ IndexTag{ rightTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ rightTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ rightTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ leftTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ leftTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ leftTag, 2 }, false };

	using namespace LI;

	GammaMatrix<Scalar> res;
	res(0, 4) = one<Scalar>();
	res(1, 3) = eta<Scalar>(nu, lambda);
	res(2, 2) = - (I<Scalar>() / Scalar{ 2 }) * epsilon<Scalar>(nu1, nu2, lambda1, lambda2);
	res(3, 1) = eta<Scalar>(nu, lambda);
	res(4, 0) = one<Scalar>();

	return res;
}

template<typename Scalar>
GammaMatrix<Scalar> sigma(const TensorIndex& mu1,
		const TensorIndex& mu2,
		int leftTag,
		int rightTag)  {
	TensorIndex nu{ IndexTag{ rightTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ rightTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ rightTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ leftTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ leftTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ leftTag, 2 }, false };

	using namespace LI;

	GammaMatrix<Scalar> res;
	res(0, 2) = eta<Scalar>(mu1, nu1) * eta<Scalar>(mu2, nu2)
			- eta<Scalar>(mu2, nu1) * eta<Scalar>(mu1, nu2);
	res(1, 1) = I<Scalar>() * (eta<Scalar>(mu1, lambda) * eta<Scalar>(mu2, nu)
			- eta<Scalar>(mu2, lambda) * eta<Scalar>(mu1, nu));
	res(1, 3) = -epsilon<Scalar>(mu1, mu2, nu, lambda);

	res(2, 0) = (one<Scalar>() / Scalar{ 2 }) *
			(eta<Scalar>(mu1, lambda1) * eta<Scalar>(mu2, lambda2)
					- eta<Scalar>(mu2, lambda1) * eta<Scalar>(mu1, lambda2));

	res(2, 2) = (I<Scalar>() / Scalar{ 2 }) * (
					eta<Scalar>(mu1, lambda1) * eta<Scalar>(nu2, lambda2) * eta<Scalar>(mu2, nu1)
					- eta<Scalar>(mu2, lambda1) * eta<Scalar>(nu2, lambda2) * eta<Scalar>(mu1, nu1)
					- eta<Scalar>(mu1, lambda1) * eta<Scalar>(nu1, lambda2) * eta<Scalar>(mu2, nu2)
					+ eta<Scalar>(mu2, lambda1) * eta<Scalar>(nu1, lambda2) * eta<Scalar>(mu1, nu2)
					+ eta<Scalar>(mu1, nu1) * eta<Scalar>(mu2, lambda2) * eta<Scalar>(nu2, lambda1)
					- eta<Scalar>(mu1, nu2) * eta<Scalar>(mu2, lambda2) * eta<Scalar>(nu1, lambda1)
					- eta<Scalar>(mu2, nu1) * eta<Scalar>(mu1, lambda2) * eta<Scalar>(nu2, lambda1)
					+ eta<Scalar>(mu2, nu2) * eta<Scalar>(mu1, lambda2) * eta<Scalar>(nu1, lambda1));

	res(2, 4) = - (I<Scalar>() / Scalar{ 2 }) * epsilon<Scalar>(mu1, mu2, lambda1, lambda2);
	res(3, 1) = - epsilon<Scalar>(mu1, mu2, nu, lambda);
	res(3, 3) = I<Scalar>() *
					(eta<Scalar>(mu2, nu) * eta<Scalar>(mu1, lambda)
					- eta<Scalar>(mu1, nu) * eta<Scalar>(mu2, lambda));
	res(4, 2) = - I<Scalar>() * epsilon<Scalar>(mu1, mu2, nu1, nu2);

	return res;
}


template<typename Scalar>
inline GammaMatrix<Scalar> diag(const LI::TensorPolynomial<Scalar>& coeff) {
	GammaMatrix<Scalar> res;
	for (unsigned int i = 0; i < 5; ++i)
		res(i, i) = coeff;

	return res;
}

} /* namespace algebra */

} /* namespace dirac */

#endif /* SRC_PSEUDOMATRIX_H_ */
