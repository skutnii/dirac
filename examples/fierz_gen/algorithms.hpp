/*
 * algorithms.hpp
 *
 *  Created on: Jan 24, 2023
 *      Author: skutnii
 */

#ifndef EXAMPLES_FIERZ_GEN_ALGORITHMS_HPP_
#define EXAMPLES_FIERZ_GEN_ALGORITHMS_HPP_

#include "defs.hpp"
#include <unordered_map>
#include "Expression.hpp"

namespace fierz {

using IndexMap = std::unordered_map<TensorIndex, TensorIndex>;

/**
 * Replace multiple indices in a tensor polynomial.
 * Replacement rules are defined by the second argument
 * whose keys are indices to replace and values are what to replace
 * the keys with.
 */
template<typename Scalar>
TensorPolynomial<Scalar>
mapIndices(const TensorPolynomial<Scalar>& src, const IndexMap& map) {
	if (map.empty())
		return src;

	TensorPolynomial<Scalar> res;
	for (const auto& srcTerm : src.terms) {
		res.terms.emplace_back(srcTerm.coeff);
		auto& destTerm = res.terms.back();

		for (const auto& srcFac : srcTerm.factors) {
			destTerm.factors.push_back(srcFac);
			auto& destFac = destTerm.factors.back();
			size_t indexCount = destFac.indices().size();
			for (size_t i = 0; i < indexCount; ++i) {
				auto indexIt = map.find(destFac.indices()[i]);
				if (indexIt != map.end())
					destFac.replaceIndex(i, indexIt->second);
			}
		}
	}

	return res;
}

//----------------------------------------------------------------------

/**
 * Get the "kernel" of a Fierz bilinear:
 * that is, the matrix expression sandwiched between spinors.
 */
GammaPolynomial<Rational> kernel(const Bilinear& bilinear);

//----------------------------------------------------------------------

/**
 * Collect terms in an expression together when allowed by symmetries.
 * ATTENTION: this algorithm presumes the expression
 * to be Lorentz-invariant.
 * If the argument has free indices, the behavior is undefined.
 */
Expression collectTerms(const Expression& src);

//----------------------------------------------------------------------

/**
 * Creates a bilinear. First parameter is used for identifier and
 * second to construct indices, and third identifies index position
 */
Bilinear taggedBilinear(int id, int tag, bool isUpper);

//----------------------------------------------------------------------

using IndexIdMap = std::unordered_map<IndexId, IndexId>;

//----------------------------------------------------------------------

/**
 * Rename indices in a bilinear
 * using second argument as replacement dictionary
 */
Bilinear renameIndices(const Bilinear& b, const IndexIdMap& repl);

//----------------------------------------------------------------------

/**
 * Rename indices in a multilinear
 * using second argument as replacement dictionary
 */
Multilinear renameIndices(const Multilinear& m, const IndexIdMap& repl);

//----------------------------------------------------------------------

/**
 * Rename indices in a tensor polynomial
 * using second argument as replacement dictionary
 */
template<typename Scalar>
TensorPolynomial<Scalar>
renameIndices(const TensorPolynomial<Scalar>& src,
		const IndexIdMap& repl) {
	TensorPolynomial<Scalar> res;
	res.terms.reserve(src.terms.size());

	for (const auto& srcTerm: src.terms) {
		res.terms.emplace_back(srcTerm.coeff);
		auto& destTerm = res.terms.back();
		for (const auto& factor: srcTerm.factors) {
			TensorIndices mappedIndices;
			mappedIndices.reserve(factor.indices().size());

			for (const TensorIndex& idx : factor.indices()) {
				auto pIdx = repl.find(idx.id);
				if (pIdx == repl.end())
					mappedIndices.push_back(idx);
				else
					mappedIndices.emplace_back(
							pIdx->second, idx.isUpper);
			}

			destTerm.factors.push_back(
					dirac::algebra::LI::Tensor::create(
								factor.id(), mappedIndices));
		}
	}

	return res;
}

//----------------------------------------------------------------------

/**
 * Get all contracted indices in a term
 */
std::vector<IndexId> contractedIndices(const Expression::Term& term);

//----------------------------------------------------------------------

/**
 * If two multilinears are equivalent up to a permutation of
 * sigma-matrix indices, returns permutation sign,
 * otherwise returns an empty optional
 */
std::optional<Complex<Rational>>
equivalenceFactor(const Multilinear& m1, const Multilinear& m2);

}

#endif /* EXAMPLES_FIERZ_GEN_ALGORITHMS_HPP_ */
