/*
 * defs.hpp
 *
 *  Created on: Jan 24, 2023
 *      Author: skutnii
 */

#ifndef EXAMPLES_FIERZ_GEN_DEFS_HPP_
#define EXAMPLES_FIERZ_GEN_DEFS_HPP_

#include "algebra/LorentzInvariant.hpp"
#include "algebra/Rational.hpp"
#include "algebra/TensorBase.hpp"
#include "algebra/IndexId.hpp"
#include "algebra/Tensorial.hpp"
#include "algebra/Gamma.hpp"
#include "algebra/Complex.hpp"
#include "ExprPrinter.hpp"

namespace fierz {

using Rational = dirac::algebra::Rational;

template<typename Coeff>
using Complex = dirac::algebra::Complex<Coeff>;

template<typename Scalar>
using TensorPolynomial = dirac::algebra::LI::TensorPolynomial<Scalar>;

using IndexId = dirac::algebra::IndexId;
using IndexTag = dirac::algebra::IndexTag;

template<typename C, typename F>
using Polynomial = dirac::algebra::Polynomial<C, F>;

struct BilinearBasis {
	static bool allows(int id) {
		return (id >= 0) && (id < 5);
	}

	static size_t maxIndexCount(int id) {
		switch (id) {
		case 1:
		case 3:
			return 1;
		case 2:
			return 2;
		default:
			return 0;
		}
	}
};

/**
 * Fierz bilinear representation:
 * a basis matrix sandwiched between spinors.
 * The identifier identifies basis matrix type
 * and indices are its tensor indices.
 */
using Bilinear =
		dirac::algebra::TensorBase<int, BilinearBasis, IndexId>;

using TensorIndex = dirac::algebra::TensorIndex;
using IndexId = dirac::algebra::IndexId;
using IndexTag = dirac::algebra::IndexTag;

template<typename Scalar>
using GammaPolynomial = dirac::algebra::GammaPolynomial<Scalar>;

template<typename Scalar>
using CanonicalExpr = dirac::algebra::CanonicalExpr<Scalar>;

template<typename Scalar>
using ExprPrinter = dirac::symbolic::ExprPrinter<Scalar>;

}

#endif /* EXAMPLES_FIERZ_GEN_DEFS_HPP_ */
