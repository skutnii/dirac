/*
 * Expression.cpp
 *
 *  Created on: Jan 24, 2023
 *      Author: skutnii
 */


#include "Expression.hpp"
#include "algorithms.hpp"
#include <algorithm>

namespace fierz {

//----------------------------------------------------------------------

Expression Expression::fierzTransformed(size_t pos) const {
	using namespace dirac::algebra;

	Expression res;
	res.terms.reserve(terms.size());

	for (const Term& term : terms) {
		if (pos + 1 >= term.factors.size()) {
			res.terms.push_back(term);
			continue;
		}

		int tag = maxIndexTag(term) + 1;

		for (int i = 0; i < 5; ++i) {
			Rational multiplier = (i == 2) ?
								Rational{ -1, 8 } : Rational{ -1, 4 };

			if (i == 3)
				multiplier = -multiplier;

			Bilinear frag = taggedBilinear(i, tag, true);
			Bilinear dual = taggedBilinear(i, tag, false);

			GammaPolynomial<Rational> triplet =
								kernel(term.factors[pos])
								* kernel(dual)
								* kernel(term.factors[pos + 1]);

			CanonicalExpr<Rational> reduced =
					reduceGamma<Rational>(triplet);

			for (size_t j = 0; j < 5; ++j) {
				TensorPolynomial coeff =
						Complex<Rational>{ multiplier, Rational{ 0 } }
						* term.coeff
						* reduced.coeffs(j);

				Term mapped{ coeff };
				mapped.factors = term.factors;
				mapped.factors[pos] = taggedBilinear(j, 0, true);
				mapped.factors[pos + 1] = frag;
				res.terms.push_back(mapped);
			}
		}
	}

	return res;
}

//----------------------------------------------------------------------

int Expression::maxIndexTag(const Expression::Term& term) {
	int max = -std::numeric_limits<int>::max();

	for (const auto& coeffTerm: term.coeff.terms)
		for (const auto& factor: coeffTerm.factors)
			for (const TensorIndex& idx : factor.indices())
				if (std::holds_alternative<IndexTag>(idx.id)) {
					const IndexTag& iTag = std::get<IndexTag>(idx.id);
					if (iTag.first > max)
						max = iTag.first;
				}

	return max;
}

}

