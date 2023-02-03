/*
 * Expression.hpp
 *
 *  Created on: Jan 23, 2023
 *      Author: skutnii
 */

#ifndef EXAMPLES_FIERZ_GEN_EXPRESSION_HPP_
#define EXAMPLES_FIERZ_GEN_EXPRESSION_HPP_

#include "algebra/LorentzInvariant.hpp"
#include "algebra/Rational.hpp"
#include "algebra/TensorBase.hpp"
#include "algebra/IndexId.hpp"
#include <vector>
#include "algebra/Polynomials.hpp"
#include "defs.hpp"

namespace fierz {

/**
 * Fierz expression structure
 */
struct Expression :
		public Polynomial<TensorPolynomial<Rational>, Bilinear> {

	using Coeff = TensorPolynomial<Rational>;
	using Base = Polynomial<TensorPolynomial<Rational>, Bilinear>;
	using Term = typename Base::Term;

	Expression operator+(const Expression& other) const {
		return dirac::algebra::sum<Expression,
									Coeff, Bilinear>(*this, other);
	}

	Expression operator-(const Expression& other) const {
		return dirac::algebra::diff<Expression,
									Coeff, Bilinear>(*this, other);
	}

	Expression operator*(const Expression& other) const {
		return dirac::algebra::prod<Expression,
									Coeff, Bilinear>(*this, other);
	}

	Expression operator-() const {
		return dirac::algebra::negate<Expression,
									  Coeff, Bilinear>(*this);
	}

	Expression fierzTransformed(size_t pos) const;

	static int maxIndexTag(const Term& term);
};

}

#endif /* EXAMPLES_FIERZ_GEN_EXPRESSION_HPP_ */
