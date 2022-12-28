/*
 * PseudoMatrix.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#include <stdexcept>

#include "GammaMatrix.hpp"
#include "IndexId.hpp"

namespace dirac {

namespace algebra {

using namespace LI;

GammaMatrix
gamma(const TensorIndex& mu, int leftTag, int rightTag) {
	TensorIndex nu{ IndexTag{ rightTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ rightTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ rightTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ leftTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ leftTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ leftTag, 2 }, false };

	GammaMatrix res;
	res(0, 1) = eta(mu, nu);
	res(1, 0) = eta(mu, lambda);
	res(1, 2) = I() * (eta(mu, nu1) * eta(nu2, lambda)
								- eta(mu, nu2) * eta(nu1, lambda));
	res(2, 1) = -(I() / 2.0) * (eta(mu, lambda1) * eta(nu, lambda2) -
			eta(mu, lambda2) * eta(nu, lambda1));
	res(2, 3) = (one() / 2.0) * epsilon(mu, nu, lambda1, lambda2);
	res(3, 2) = epsilon(mu, nu1, nu2, lambda);
	res(3, 4) = -eta(mu, lambda);
	res(4, 3) = -eta(mu, nu);

	return res;
}

GammaMatrix
gamma5(int leftTag, int rightTag) {
	TensorIndex nu{ IndexTag{ rightTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ rightTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ rightTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ leftTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ leftTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ leftTag, 2 }, false };

	GammaMatrix res;
	res(0, 4) = one();
	res(1, 3) = eta(nu, lambda);
	res(2, 2) = - (I() / 2.0) * epsilon(nu1, nu2, lambda1, lambda2);
	res(3, 1) = eta(nu, lambda);
	res(4, 0) = one();

	return res;
}

GammaMatrix
sigma(const TensorIndex& mu1, const TensorIndex &mu2,
		int leftTag, int rightTag) {
	TensorIndex nu{ IndexTag{ rightTag, 0 }, true };
	TensorIndex nu1{ IndexTag{ rightTag, 1 }, true };
	TensorIndex nu2{ IndexTag{ rightTag, 2 }, true };

	TensorIndex lambda{ IndexTag{ leftTag, 0 }, false };
	TensorIndex lambda1{ IndexTag{ leftTag, 1 }, false };
	TensorIndex lambda2{ IndexTag{ leftTag, 2 }, false };

	GammaMatrix res;
	res(0, 2) = eta(mu1, nu1) * eta(mu2, nu2) - eta(mu2, nu1) * eta(mu1, nu2);
	res(1, 1) = I() * (eta(mu1, lambda) * eta(mu2, nu)
			- eta(mu2, lambda) * eta(mu1, nu));
	res(1, 3) = epsilon(mu1, mu2, nu, lambda);
	res(2, 0) = (1.0 / 2.0) *
			(eta(mu1, lambda1) * eta(mu2, lambda2)
					- eta(mu2, lambda1) * eta(mu1, lambda2));
	res(2, 4) = - (I() / 2.0) * epsilon(mu1, mu2, lambda1, lambda2);
	res(3, 1) = (1.0 / 2.0) * epsilon(mu1, mu2, nu, lambda);
	res(3, 3) = (I() / 2.0)
			* (eta(mu1, nu) * eta(mu2, lambda)
					- eta(mu2, nu) * eta(mu1, lambda));
	res(4, 2) = (I() / 2.0) * epsilon(mu1, mu2, nu1, nu2);

	return res;
}

} /* namespace algebra */

} /* namespace dirac */
