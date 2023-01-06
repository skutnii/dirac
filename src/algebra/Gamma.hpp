/*
 * Gamma.hpp
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

struct GammaBasis {
	GammaBasis() = default;

	static const std::string gamma;
	static const std::string sigma;
	static const std::string gamma5;

	using NameSet = std::unordered_set<std::string>;
	static const NameSet Elements;

	inline static bool allows(const std::string& id) {
		return (algebra::LI::Basis::allows(id)
				|| (Elements.find(id) != Elements.end()));
	}

	inline static size_t maxIndexCount(const std::string& id) {
		if (algebra::LI::Basis::allows(id))
			return algebra::LI::Basis::maxIndexCount(id);

		if (id == sigma)
			return 2;

		if (id == gamma)
			return 1;

		return 0;
	}
};

using GammaTensor =
		TensorBase<std::string, GammaBasis, IndexId>;

template<typename Scalar>
using GammaPolynomial = Polynomial<Complex<Scalar>, GammaTensor>;

template<typename Scalar>
struct CanonicalExpr {
	GammaVector<Scalar> coeffs;

	TensorIndex vectorIndex;
	std::pair<TensorIndex, TensorIndex> tensorIndices;
	TensorIndex pseudoVectorIndex;

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

};

template<typename Scalar>
GammaPolynomial<Scalar> toPolynomial(const GammaTensor& t) {
	typename GammaPolynomial<Scalar>::Term term;
	term.coeff = algebra::one<Scalar>();
	term.factors.push_back(t);

	GammaPolynomial<Scalar> res;
	res.terms.push_back(term);

	return res;
}

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

template<typename Scalar>
inline GammaPolynomial<Scalar>
operator+(const GammaPolynomial<Scalar>& p1,
		const GammaPolynomial<Scalar>& p2) {
	return algebra::sum<GammaPolynomial<Scalar>,
			algebra::Complex<Scalar>, GammaTensor>(p1, p2);
}

template<typename Scalar>
inline GammaPolynomial<Scalar>
operator-(const GammaPolynomial<Scalar>& p1,
		const GammaPolynomial<Scalar>& p2) {
	return algebra::diff<GammaPolynomial<Scalar>,
			algebra::Complex<Scalar>, GammaTensor>(p1, p2);
}

template<typename Scalar>
inline GammaPolynomial<Scalar>
operator-(const GammaPolynomial<Scalar>& p) {
	return algebra::negate<GammaPolynomial<Scalar>,
			algebra::Complex<Scalar>, GammaTensor>(p);
}

template<typename Scalar>
inline GammaPolynomial<Scalar>
operator*(const GammaPolynomial<Scalar>& p1,
		const GammaPolynomial<Scalar>& p2) {
	return algebra::prod<GammaPolynomial<Scalar>,
			algebra::Complex<Scalar>, GammaTensor>(p1, p2);
}

template<typename Scalar>
inline GammaPolynomial<Scalar>
operator*(const algebra::Complex<Scalar>& c,
		const GammaPolynomial<Scalar>& p) {
	return algebra::prod<GammaPolynomial<Scalar>,
			algebra::Complex<Scalar>, GammaTensor>(c, p);
}

template<typename Scalar>
inline GammaPolynomial<Scalar>
operator*(const GammaPolynomial<Scalar>& p,
		const algebra::Complex<Scalar>& c) {
	return algebra::prod<GammaPolynomial<Scalar>,
			algebra::Complex<Scalar>, GammaTensor>(p, c);
}

} /* namespace algebra */

} /* namespace dirac */



#endif /* SRC_SYMBOLIC_HPP_ */
