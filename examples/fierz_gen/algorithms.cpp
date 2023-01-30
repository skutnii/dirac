/*
 * algorithms.cpp
 *
 *  Created on: Jan 24, 2023
 *      Author: skutnii
 */

#include "algorithms.hpp"
#include <algorithm>
#include <unordered_set>
#include <sstream>

namespace fierz {

//----------------------------------------------------------------------

GammaPolynomial<Rational> kernel(const Bilinear& bilinear) {
	using namespace dirac::algebra;

	switch (bilinear.id()) {
	case 0:
		return one<Rational>();
	case 1:
		return toPolynomial<Rational>(
			GammaTensor::create(GammaBasis::gamma,
									bilinear.indices()));
	case 2:
		return toPolynomial<Rational>(
			GammaTensor::create(GammaBasis::sigma,
									bilinear.indices()));
	case 3: {
		GammaTensor gamma = GammaTensor::create(GammaBasis::gamma,
									bilinear.indices());
		GammaTensor gamma5 =
				GammaTensor::create(GammaBasis::gamma5, {});
		return toPolynomial<Rational>(gamma5)
				* toPolynomial<Rational>(gamma);
	}
	case 4:
		return toPolynomial<Rational>(
			GammaTensor::create(GammaBasis::gamma5, {}));
	default:
		throw std::runtime_error{ "Unknown bilinear ID" };
	}

	return GammaPolynomial<Rational>{};
}

//----------------------------------------------------------------------

static std::optional<Expression::Term>
tryMerge(const Expression::Term& t1, const Expression::Term& t2) {
	size_t factorCount = t1.factors.size();
	if (t2.factors.size() != factorCount)
		return std::optional<Expression::Term>{};

	IndexMap iMap;
	for (size_t i = 0; i < factorCount; ++i) {
		const Bilinear& f1 = t1.factors[i];
		const Bilinear& f2 = t2.factors[i];

		if ((f1.id() != f2.id()) ||
				(f1.indices().size() != f2.indices().size()))
			return std::optional<Expression::Term>{};

		size_t indexCount = f1.indices().size();
		for (size_t j = 0; j < indexCount; ++j) {
			TensorIndex from = f2.indices()[j];
			from.isUpper = !from.isUpper;
			TensorIndex to = f1.indices()[j];
			to.isUpper = !to.isUpper;

			iMap.emplace(from, to);
		}
	}

	Expression::Term res{ t1.coeff + mapIndices(t2.coeff, iMap) };
	res.factors = t1.factors;

	return res;
}

//----------------------------------------------------------------------

Expression collectTerms(const Expression& src) {
	Expression res{ src };
	for (Expression::Term& term : res.terms)
		std::sort(term.factors.begin(), term.factors.end(),
				[](const Bilinear& f1, const Bilinear& f2) {
					return (f1.id() < f2.id());
				});

	std::vector<Expression::Term> tmpTerms;
	std::unordered_set<size_t> indices;
	size_t termCount = res.terms.size();
	indices.reserve(termCount);
	for (size_t i = 0; i < termCount; ++i)
		indices.insert(i);

	while (!indices.empty()) {
		size_t i = *indices.begin();
		indices.erase(i);
		std::unordered_set<size_t> rest;
		rest.reserve(indices.size());

		Expression::Term term = res.terms[i];
		for (size_t j : indices) {
			std::optional<Expression::Term> mergeRes =
							tryMerge(term, res.terms[j]);

			if (mergeRes)
				term = mergeRes.value();
			else
				rest.insert(j);
		}

		tmpTerms.push_back(term);
		indices = rest;
	}

	res.terms = tmpTerms;
	return res;
}

//----------------------------------------------------------------------

Bilinear taggedBilinear(int id, int tag, bool isUpper) {
	switch (id) {
	case 0:
	case 4:
		return Bilinear::create(id, {});
	case 1:
	case 3: {
		TensorIndex idx{ IndexTag{ tag, 0 }, isUpper };
		return Bilinear::create(id, { idx });
	}
	case 2: {
		TensorIndex idx1{ IndexTag{ tag, 1 }, isUpper };
		TensorIndex idx2{ IndexTag{ tag, 2 }, isUpper };
		return Bilinear::create(id, { idx1, idx2 });
	}
	default:
		throw std::runtime_error{ "Invalid bilinear ID" };
	}
}

}

