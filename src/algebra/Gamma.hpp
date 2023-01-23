/*
 * Gamma.hpp
 *
 * Definitions and operations on expressions containing gamma-matrices
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */

#ifndef SRC_SYMBOLIC_HPP_
#define SRC_SYMBOLIC_HPP_

#include "TensorBase.hpp"
#include "LorentzInvariant.hpp"
#include <string>
#include <unordered_set>
#include "Polynomials.hpp"
#include <ostream>
#include <vector>
#include "GammaMatrix.hpp"

namespace dirac {

namespace algebra {

/**
 * Gamma ring basis consists of basis matrices:
 * \gamma^\mu, \sigma^{\mu\nu}, and \gamma^5,
 * and Lorentz-invariant symbols: metric, Kronecker and Levi-Civita
 */
struct GammaBasis {
	GammaBasis() = default;

	static const std::string gamma;
	static const std::string sigma;
	static const std::string gamma5;

	using NameSet = std::unordered_set<std::string>;
	static const NameSet Elements;

	/**
	 * Returns true if the argument identifies one of basis elements,
	 * false otherwise
	 */
	inline static bool allows(const std::string& id) {
		return (LI::Basis::allows(id)
				|| (Elements.find(id) != Elements.end()));
	}

	/**
	 * Maximum number of indices that a basis element can have
	 */
	inline static size_t maxIndexCount(const std::string& id) {
		if (LI::Basis::allows(id))
			return LI::Basis::maxIndexCount(id);

		if (id == sigma)
			return 2;

		if (id == gamma)
			return 1;

		return 0;
	}
};

/**
 * Basis element of the gamma ring
 */
using GammaTensor =
		TensorBase<std::string, GammaBasis, IndexId>;

/**
 * Gamma-ring element, that is, a polynomial of
 * Dirac matrices and Lorentz-invariant symbols
 * with complex coefficients.
 */
template<typename Scalar>
using GammaPolynomial = Polynomial<Complex<Scalar>, GammaTensor>;

/**
 * Canonical gamma-expression is a linear combination
 * of the following matrices: 1, \gamma^\mu, \sigma^{\mu\nu},
 * \gamma^5\gamma^\mu, \gamma^5 with Lorentz-invariant coefficients
 */
template<typename Scalar>
struct CanonicalExpr {
	/**
	 * Coefficients at basis matrices:
	 * - coeffs(0) - at 1;
	 * - coeffs(1) - at \gamma^\mu;
	 * - coeffs(2) - at \sigma^{\mu\nu};
	 * - coeffs(3) - at \gamma^5\gamma^\mu;
	 * - coeffs(4) - at \gamma^5.
	 */
	GammaVector<Scalar> coeffs;

	/**
	 * Lorentz index of \gamma basis matrix
	 */
	TensorIndex vectorIndex;

	/**
	 * Lorentz indices of \sigma basis matrix
	 */
	std::pair<TensorIndex, TensorIndex> tensorIndices;

	/**
	 * Lorentz index of \gamma^5\gamma basis matrix
	 */
	TensorIndex pseudoVectorIndex;

	/**
	 * Constructs a canonical exprression from basis matrix indices
	 */
	CanonicalExpr(const TensorIndex& vector,
			const TensorIndex& tensor1,
			const TensorIndex& tensor2,
			const TensorIndex& pseudoVector) :
				vectorIndex{ vector },
				tensorIndices{ tensor1, tensor2 },
				pseudoVectorIndex{ pseudoVector } {}

	CanonicalExpr() :
		CanonicalExpr{
			TensorIndex{ IndexTag{ 0, 0 }, true },

			TensorIndex{ IndexTag{ 0, 1 }, true },
			TensorIndex{ IndexTag{ 0, 2 }, true },

			TensorIndex{ IndexTag{ 0, 0 }, true } }
	{};

	/**
	 * Merge the terms in the coefficient at \sigma,
	 * taking antisymmetry into account.
	 */
	void applySymmetry();

	/**
	 * Check whether the expression is scalar equal to the argument
	 */
	bool isScalar(Scalar s) const;

	/**
	 * Check whether the expression is zero
	 */
	bool isZero() const {
		for (unsigned int i = 0; i < 5; ++i)
			if (!coeffs(i).isZero())
				return false;

		return true;
	}
};

//----------------------------------------------------------------------

/**
 * Promote a gamma-basis element to a polynomial
 */
template<typename Scalar>
GammaPolynomial<Scalar> toPolynomial(const GammaTensor& t) {
	typename GammaPolynomial<Scalar>::Term term;
	term.coeff = one<Scalar>();
	term.factors.push_back(t);

	GammaPolynomial<Scalar> res;
	res.terms.push_back(term);

	return res;
}

//----------------------------------------------------------------------

/**
 * Transforms an arbitrary gamma polynomial to canonical form
 * by expanding products of \gamma matrices.
 */
template<typename Scalar>
CanonicalExpr<Scalar> reduceGamma(const GammaPolynomial<Scalar>& p) {
	using namespace algebra;

	CanonicalExpr<Scalar> expr;

	for (const typename GammaPolynomial<Scalar>::Term& term : p.terms) {
		LI::TensorPolynomial<Scalar> coeff{ term.coeff };
		coeff.terms[0].factors.reserve(term.factors.size());

		//Build coefficient and the list of terms
		int gammaCount = 0;
		std::vector<GammaMatrix<Scalar>> factorsRepr;
		factorsRepr.reserve(term.factors.size());
		for (const GammaTensor& factor : term.factors) {
			if (LI::Basis::allows(factor.id()))
				coeff *= LI::Tensor::create(factor.id(),
						factor.indices());
			else {
				if (!factor.complete())
					throw std::runtime_error{
						"Not enough indices for " + factor.id() };

				int nextCount = gammaCount + 1;
				const TensorIndices& indices = factor.indices();
				if (GammaBasis::gamma == factor.id())
					factorsRepr.push_back(
							gamma<Scalar>(indices[0],
									gammaCount, nextCount));
				else if (GammaBasis::sigma == factor.id())
					factorsRepr.push_back(
							sigma<Scalar>(indices[0],
									indices[1],
									gammaCount,
									nextCount));
				else if (GammaBasis::gamma5 == factor.id())
					factorsRepr.push_back(
							gamma5<Scalar>(gammaCount, nextCount));
				else
					throw std::runtime_error{
						"Unknown tensor name: " + factor.id() };

				gammaCount = nextCount;
			}
		}

		//Multiply terms from right to left
		if (factorsRepr.empty())
			expr.coeffs += GammaVector<Scalar>{ coeff,
				LI::ZeroPoly<Scalar>(), LI::ZeroPoly<Scalar>(),
				LI::ZeroPoly<Scalar>(), LI::ZeroPoly<Scalar>() };
		else {
			GammaVector<Scalar> termRepr = factorsRepr.back().col(0);
			for (auto iFactor = factorsRepr.rbegin() + 1;
					iFactor != factorsRepr.rend(); ++iFactor)
				termRepr = (*iFactor) * termRepr;

			expr.coeffs += coeff * termRepr;
		}
	}

	return expr;
}

//----------------------------------------------------------------------

//Gamma polynomial sum
template<typename Scalar>
inline GammaPolynomial<Scalar>
operator+(const GammaPolynomial<Scalar>& p1,
		const GammaPolynomial<Scalar>& p2) {
	return sum<GammaPolynomial<Scalar>,
			Complex<Scalar>, GammaTensor>(p1, p2);
}

//----------------------------------------------------------------------

//Gamma polynomial difference
template<typename Scalar>
inline GammaPolynomial<Scalar>
operator-(const GammaPolynomial<Scalar>& p1,
		const GammaPolynomial<Scalar>& p2) {
	return diff<GammaPolynomial<Scalar>,
			Complex<Scalar>, GammaTensor>(p1, p2);
}

//----------------------------------------------------------------------

//Gamma polynomial negation (unary minus) operator
template<typename Scalar>
inline GammaPolynomial<Scalar>
operator-(const GammaPolynomial<Scalar>& p) {
	return negate<GammaPolynomial<Scalar>,
			Complex<Scalar>, GammaTensor>(p);
}

//----------------------------------------------------------------------

//Product of gamma polynomials
template<typename Scalar>
inline GammaPolynomial<Scalar>
operator*(const GammaPolynomial<Scalar>& p1,
		const GammaPolynomial<Scalar>& p2) {
	return prod<GammaPolynomial<Scalar>,
			Complex<Scalar>, GammaTensor>(p1, p2);
}

//----------------------------------------------------------------------

//left multiplication of a gamma polynomial by a complex number
template<typename Scalar>
inline GammaPolynomial<Scalar>
operator*(const Complex<Scalar>& c,
		const GammaPolynomial<Scalar>& p) {
	return prod<GammaPolynomial<Scalar>,
			Complex<Scalar>, GammaTensor>(c, p);
}

//----------------------------------------------------------------------

//Right multiplication of a gamma polynomial by a complex number
template<typename Scalar>
inline GammaPolynomial<Scalar>
operator*(const GammaPolynomial<Scalar>& p,
		const Complex<Scalar>& c) {
	return prod<GammaPolynomial<Scalar>,
			Complex<Scalar>, GammaTensor>(p, c);
}

//----------------------------------------------------------------------

template<typename Scalar>
void CanonicalExpr<Scalar>::applySymmetry() {
	using Term = typename LI::TensorPolynomial<Scalar>::Term;
	coeffs(2).mergeTerms(
			[&](const Term& t1, const Term& t2) -> std::optional<Term> {
				Term swapped{ t2 };
				swapped.coeff = -swapped.coeff;

				//Swap indices
				TensorIndex i1{ this->tensorIndices.first.id,
								!this->tensorIndices.first.isUpper };
				TensorIndex i2{ this->tensorIndices.second.id,
								!this->tensorIndices.second.isUpper };
				for (LI::Tensor& factor : swapped.factors) {
					const TensorIndices& indices = factor.indices();
					for (size_t i = 0; i < indices.size(); ++i)
						if (indices[i] == i1)
							factor.replaceIndex(i, i2);
						else if (indices[i] == i2)
							factor.replaceIndex(i, i1);
				}

				return LI::TensorPolynomial<Scalar>::tryMerge(
														t1, swapped);
			});
}

//----------------------------------------------------------------------

template<typename Scalar>
bool CanonicalExpr<Scalar>::isScalar(Scalar s) const {
	for (unsigned int i = 1; i < 5; ++i)
		if (!coeffs(i).isZero())
			return false;

	return ((coeffs(0).terms.size() == 1)
			&& coeffs(0).terms[0].factors.empty()
			&& (coeffs(0).terms[0].coeff == s));
}

} /* namespace algebra */

} /* namespace dirac */


#endif /* SRC_SYMBOLIC_HPP_ */
