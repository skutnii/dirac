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

	std::vector<IndexId> contracted1 = contractedIndices(t1);
	std::vector<IndexId> contracted2 = contractedIndices(t2);

	size_t dummyCount = contracted1.size();
	if (dummyCount != contracted2.size())
		return std::optional<Expression::Term>{};

	using namespace dirac::algebra;

	if (dummyCount == 0) {
		std::optional<Complex<Rational> > equiv =
							equivalenceFactor(t1.factors, t2.factors);

		if (!equiv)
			return std::optional<Expression::Term>{};

		Expression::Term res{ t1 };
		res.coeff += equiv.value() * t2.coeff;

		return res;
	}

	std::optional<IndexIdMap> iMap;
	Complex<Rational> factor = one<Rational>();
	forPermutations(dummyCount,
			[&](const Permutation& perm) {
				if (iMap)
					return;

				IndexIdMap maybeMap;
				for (size_t i = 0; i < dummyCount; ++i)
					maybeMap.emplace(contracted2[i],
							contracted1[perm.map[i]]);

				Multilinear mapped =
						renameIndices(t2.factors, maybeMap);
				std::optional<Complex<Rational> > equiv =
						equivalenceFactor(mapped, t1.factors);
				if (equiv) {
					iMap = maybeMap;
					factor = equiv.value();
				}
			});

	if (!iMap)
		return std::optional<Expression::Term>{};

	Expression::Term res{ t1 };
	res.coeff +=
			factor * renameIndices<Rational>(t2.coeff, iMap.value());

	return res;
}

//----------------------------------------------------------------------

Expression collectTerms(const Expression& src) {
	Expression res;

	std::unordered_set<size_t> indices;
	size_t termCount = src.terms.size();
	indices.reserve(termCount);
	for (size_t i = 0; i < termCount; ++i)
		indices.insert(i);

	while (!indices.empty()) {
		size_t i = *indices.begin();
		indices.erase(i);
		std::unordered_set<size_t> rest;
		rest.reserve(indices.size());

		Expression::Term term = src.terms[i];
		for (size_t j : indices) {
			std::optional<Expression::Term> mergeRes =
							tryMerge(term, src.terms[j]);

			if (mergeRes)
				term = mergeRes.value();
			else
				rest.insert(j);
		}

		res.terms.push_back(term);
		indices = rest;
	}

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

//----------------------------------------------------------------------

Bilinear renameIndices(const Bilinear& b, const IndexIdMap& repl) {
	TensorIndices mapped;
	mapped.reserve(b.indices().size());

	for (const TensorIndex& idx: b.indices()) {
		auto iter = repl.find(idx.id);
		if (iter == repl.end())
			mapped.push_back(idx);
		else
			mapped.emplace_back(iter->second, idx.isUpper);
	}

	return Bilinear::create(b.id(), mapped);
}

//----------------------------------------------------------------------

Multilinear
renameIndices(const Multilinear& m, const IndexIdMap& repl) {
	Multilinear res;
	res.reserve(m.size());
	for (const Bilinear& b: m)
		res.push_back(renameIndices(b, repl));

	return res;
}

//----------------------------------------------------------------------

using IndexIdSet = std::unordered_set<IndexId>;

std::vector<IndexId> contractedIndices(const Expression::Term& term) {
	std::unordered_set<TensorIndex> freeIndices;
	IndexIdSet res;

	for (const Bilinear& b : term.factors)
		for (const TensorIndex& idx: b.indices()) {
			TensorIndex dual{ idx.id, !idx.isUpper };
			if (freeIndices.find(dual) != freeIndices.end()) {
				res.insert(idx.id);
				freeIndices.erase(dual);
			}

			freeIndices.insert(idx);
		}


	if (!term.coeff.terms.empty()) {
		std::unordered_set<TensorIndex> coeffIndices;

		for (const auto& factor: term.coeff.terms[0].factors)
			coeffIndices.insert(
					factor.indices().begin(),
					factor.indices().end());

		for (const TensorIndex& idx: coeffIndices) {
			TensorIndex dual{ idx.id, !idx.isUpper };
			if (freeIndices.find(dual) != freeIndices.end())
				res.insert(idx.id);
		}
	}

	return std::vector<IndexId>{ res.begin(), res.end() };
}

//----------------------------------------------------------------------

std::optional<Complex<Rational> >
equivalenceFactor(const Multilinear& m1, const Multilinear &m2) {
	if (m1.size() != m2.size())
		return std::optional<Complex<Rational> >{};

	Complex<Rational> factor = dirac::algebra::one<Rational>();

	Multilinear factors{ m2.begin(), m2.end() };
	for (const Bilinear& b1 : m1) {
		auto pb2 = std::find_if(factors.begin(), factors.end(),
				[&](const Bilinear& b2) -> bool {
					if (b2 == b1)
						return true;

					if ((b2.id() == b1.id()) && (b1.id() == 2)) {
						if ((b1.indices().size() != 2)
								|| (b2.indices().size() != 2))
							throw std::runtime_error{
									"Invalid index count" };

						if ((b1.indices()[0] == b2.indices()[1])
							&& (b1.indices()[1] == b2.indices()[0])) {
							factor *= - dirac::algebra::one<Rational>();
							return true;
						}
					}

					return false;
				});

		if (pb2 == factors.end())
			return std::optional<Complex<Rational> >{};
		else
			factors.erase(pb2);
	}

	return factor;
}

}

