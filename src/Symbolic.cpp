/*
 * Symbolic.cpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */


#include "Symbolic.hpp"

#include "algebra/GammaMatrix.hpp"

namespace dirac {

namespace symbolic {

const std::string Basis::gamma = "\\gamma";
const std::string Basis::sigma = "\\sigma";
const std::string Basis::gamma5 = "\\gamma5";

const Basis::NameSet Basis::Elements{
	Basis::gamma,
	Basis::gamma5,
	Basis::sigma
};

GammaPolynomial toPolynomial(const Tensor& t) {
	GammaPolynomial::Term term;
	term.coeff = algebra::one();
	term.factors.push_back(t);

	GammaPolynomial res;
	res.terms.push_back(term);

	return res;
}

using namespace algebra;

CanonicalExpr reduceGamma(const GammaPolynomial& p) {
	CanonicalExpr expr;

	for (const GammaPolynomial::Term& term : p.terms) {
		LI::TensorPolynomial coeff{ term.coeff };
		coeff.terms[0].factors.reserve(term.factors.size());

		//Build coefficient and the list of terms
		int gammaCount = 0;
		std::vector<GammaMatrix> factorsRepr;
		factorsRepr.reserve(term.factors.size());
		for (const Tensor& factor : term.factors) {
			if (LI::Basis::allows(factor.id()))
				coeff * LI::Tensor::create(factor.id(), factor.indices());
			else {
				if (!factor.complete())
					throw std::runtime_error{ "Not enough indices for " + factor.id() };

				int nextCount = gammaCount + 1;
				const TensorIndices& indices = factor.indices();
				if (Basis::gamma == factor.id())
					factorsRepr.push_back(gamma(indices[0], gammaCount, nextCount));
				else if (Basis::sigma == factor.id())
					factorsRepr.push_back(sigma(indices[0], indices[1], gammaCount, nextCount));
				else if (Basis::gamma5 == factor.id())
					factorsRepr.push_back(gamma5(gammaCount, nextCount));
				else
					throw std::runtime_error{ "Unknown tensor name: " + factor.id() };

				gammaCount = nextCount;
			}
		}

		//Multiply terms from right to left
		if (factorsRepr.empty())
			expr.coeffs += GammaVector{ coeff,
				LI::ZeroPoly(), LI::ZeroPoly(),
				LI::ZeroPoly(), LI::ZeroPoly() };
		else {
			GammaVector termRepr = factorsRepr.back().col(0);
			for (auto iFactor = factorsRepr.rbegin() + 1;
					iFactor != factorsRepr.rend(); ++iFactor)
				termRepr = (*iFactor) * termRepr;

			expr.coeffs += coeff * termRepr;
		}
	}

	return expr;
}

}

}

