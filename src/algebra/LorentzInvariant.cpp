/*
 * Tensor.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#include <stdexcept>
#include <algorithm>
#include "LorentzInvariant.hpp"

namespace dirac {

namespace algebra {

namespace LI {

const std::string Basis::eta{ "\\eta" };
const std::string Basis::epsilon{ "\\epsilon" };
const std::string Basis::delta{ "\\delta" };

const Basis::NameSet Basis::Elements {
	Basis::eta,
	Basis::delta,
	Basis::epsilon
};

void TensorPolynomial::canonicalize() {

}

Tensor eta(const TensorIndex& mu, const TensorIndex& nu) {
	std::string id = (mu.isUpper == nu.isUpper) ?
			Basis::eta : Basis::delta;
	return Tensor::create(id, Tensor::Indices{ mu, nu });
}

Tensor epsilon(const TensorIndex &kappa, const TensorIndex &lambda,
		const TensorIndex &mu, const TensorIndex &nu) {
	return Tensor::create(Basis::epsilon,
			 Tensor::Indices{ kappa, lambda, mu, nu });
}

TensorPolynomial operator*(const Tensor& t1, const Tensor& t2) {
	TensorPolynomial res;
	res.terms.emplace_back(one());
	TensorPolynomial::Term& term = res.terms.back();
	term.factors.push_back(t1);
	term.factors.push_back(t2);

	res.canonicalize();
	return res;
}

} /* namespace LI */

} //Namespace algebra

} //namespace dirac


