/*
 * Symbolic.cpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */


#include "Symbolic.hpp"

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

CanonicalExpr reduceGamma(const GammaPolynomial& p) {
	return CanonicalExpr{}; //TODO: implement!
}

}

}

