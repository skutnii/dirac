/*
 * LorentzInvariant.hpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_ALGEBRA_LORENTZINVARIANT_HPP_
#define SRC_ALGEBRA_LORENTZINVARIANT_HPP_

#include <unordered_set>
#include <string>

#include "TensorBase.hpp"
#include <complex>
#include "Polynomials.hpp"
#include "Complex.hpp"
#include "Rational.hpp"
#include "Permutations.hpp"
#include <algorithm>

namespace dirac {

namespace algebra {

//Lorentz invariant tensors namespace
namespace LI {

/**
 * Basis of Lorentz-invariant (pseudo)-tensor ring.
 * Consists of metric, Kronecker delta, and Levi-Civita symbol.
 */
struct Basis {
	Basis() = default;

	static const std::string eta;
	static const std::string epsilon;
	static const std::string delta;

	using NameSet = std::unordered_set<std::string>;
	static const NameSet Elements;

	/**
	 * returns true if the argument identifies one of basis elements,
	 * false otherwise
	 */
	inline static bool allows(const std::string& id) {
		return (Elements.find(id) != Elements.end());
	}

	/**
	 * Maximum number of indices a basis element can have
	 */
	inline static size_t maxIndexCount(const std::string& id) {
		if (id == epsilon)
			return 4;

		if ((id == delta)
			|| (id == eta))
			return 2;

		return 0;
	}

};

/**
 * Basis Lorentz-invariant (pseudo)-tensor ring element
 */
using Tensor = TensorBase<std::string, Basis, IndexId>;

/**
 * Lorentz-invariant (pseudo)-tensor polynomial type
 */
template<typename Scalar>
struct TensorPolynomial : public Polynomial<Complex<Scalar>, Tensor> {
	using Coeff = Complex<Scalar>;
	using Base = Polynomial<Complex<Scalar>, Tensor>;
	using Term = typename Base::Term;

	/**
	 * Addition operator
	 */
	TensorPolynomial<Scalar>
	operator+(const TensorPolynomial<Scalar>& other) const {
		return sum<TensorPolynomial<Scalar>,
					Coeff, Tensor>(*this, other);
	}

	/**
	 * Subtraction operator
	 */
	TensorPolynomial<Scalar>
	operator-(const TensorPolynomial<Scalar>& other) const {
		return diff<TensorPolynomial<Scalar>,
					Coeff, Tensor>(*this, other);
	}

	/**
	 * Multiplication operator
	 */
	TensorPolynomial<Scalar>
	operator*(const TensorPolynomial<Scalar>& other) const {
		return prod<TensorPolynomial<Scalar>,
					Coeff, Tensor>(*this, other);
	}

	/**
	 * Right multiplication by a complex coefficient
	 */
	TensorPolynomial<Scalar> operator*(const Coeff& c) const {
		return prod<TensorPolynomial<Scalar>,
					Coeff, Tensor>(*this, c);
	}

	/**
	 * Negation (unary minus) operator
	 */
	TensorPolynomial<Scalar> operator-() const {
		return negate<TensorPolynomial<Scalar>, Coeff, Tensor>(*this);
	}

	/**
	 * Mutating addition
	 */
	TensorPolynomial<Scalar>& operator+=(
			const TensorPolynomial<Scalar>& other) {
		return add<TensorPolynomial<Scalar>,
					Coeff, Tensor>(*this, other);
	}

	/**
	 * Mutating subtraction
	 */
	TensorPolynomial<Scalar>& operator-=(
			const TensorPolynomial<Scalar>& other) {
		return sub<TensorPolynomial<Scalar>,
					Coeff, Tensor>(*this, other);
	}

	/**
	 * Mutating multiplication
	 */
	TensorPolynomial<Scalar>& operator*=(
			const TensorPolynomial<Scalar>& other) {
		this->terms = (*this * other).terms;
		return *this;
	}

	/**
	 * Mutating right multiplication by a basis (pseudo)-tensor
	 */
	TensorPolynomial<Scalar>& operator*=(const Tensor& t) {
		for (Term& term : this->terms)
			term.factors.push_back(t);

		return *this;
	}

	/**
	 * Mutating right multiplication by a complex number
	 */
	TensorPolynomial<Scalar>& operator*=(const Coeff& c) {
		for (Term& term : this->terms)
			term.coeff = term.coeff * c;

		return *this;
	}

	TensorPolynomial() = default;
	TensorPolynomial(const TensorPolynomial<Scalar>& other) = default;
	TensorPolynomial(TensorPolynomial<Scalar>&& other) = default;

	/**
	 * Assignment
	 */
	TensorPolynomial&
	operator=(const TensorPolynomial<Scalar>& other) = default;

	/**
	 * Promote a complex number to a zer-rank tensor polynomial
	 */
	TensorPolynomial(const Coeff& c)
		: Base{} {
		if ((c.real() != 0) || (c.imag() != 0))
			this->terms.emplace_back(c);
	}

	/**
	 * Promote a scalar to a zero-rank tensor polynomial
	 */
	TensorPolynomial(const Scalar& s) :
		TensorPolynomial{ Coeff{ s, static_cast<Scalar>(0) } } {}

	/**
	 * Promote a basis (pseudo)-tensor to a zero-rank tensor polynomial
	 */
	TensorPolynomial(const Tensor& t) : Base{} {
		this->terms.emplace_back(one<Scalar>(), t);
	}

	/**
	 * Main canonicalization procedure.
	 * Canonicalization is performed in three steps:
	 * 1) powers of Levi-Civita symbol are expanded
	 * 	  as products of metric and/or Kronecker symbols;
	 * 2) all contractible Lorentz indices are contracted;
	 * 3) all mergeable terms are merged
	 */
	void canonicalize() override;

	/**
	 * Check if the polynomial is zero
	 */
	bool isZero() const {
		if (this->terms.empty())
			return true;

		for (const Term& term : this->terms)
			if (term.coeff != zero<Scalar>())
				return false;

		return true;
	}

	/**
	 * Contract all contractible Lorentz indices in a term.
	 * If the resulting term is zero (which happens if the coefficient
	 * is zero or indices in a Levi-Civita symbol are contracted),
	 * returns an empty optional.
	 */
	static std::optional<Term> contractIndices(const Term& src);

	/**
	 * If two term have same tensorial structure up to a permutation of
	 * factors' indices returns the merger of the terms, that is,
	 * the term with the same factors and the coefficient being sum or
	 * difference of arguments' coefficients depending on whether
	 * the Levi-Civita symbols in the arguments are related
	 * by an even or odd index permutation.
	 *
	 * If the terms are not mergeable, returns an empty optional.
	 */
	static std::optional<Term> tryMerge(const Term& t1, const Term& t2);

	/**
	 * Expands products of Levi-Civita symbols into sums of products
	 * of metric and/or Kronecker symbols. After the invocation of
	 * this method, the polynomial is at most linear
	 * in Levi-Civita symbol.
	 */
	void expandEpsilonPowers();

	/**
	 * Contracts all contractible Lorentz indices in each callee's term.
	 */
	void contractIndices();

	using Merger =
			std::function<
				std::optional<Term> (const Term& t1, const Term& t2)>;

	/**
	 * Tries merging all mergeable terms using the user-provided
	 * merger function. The function must return merging result
	 * for two mergeable terms or an empty optional
	 * if the terms are not mergeable
	 */
	void mergeTerms(Merger merger = tryMerge);
};

} /* namespace LI */

} /* namespace algebra */

} /* namespace dirac */

namespace std {

/**
 * Hash specialization for Lorentz-invariant (pseudo)-tensors
 */
template<>
struct hash<dirac::algebra::LI::Tensor> {
	size_t operator()(const dirac::algebra::LI::Tensor& t) const {
		size_t value = std::hash<std::string>{}(t.id());
		for (const auto& idx: t.indices())
			value = value
					^ std::hash<dirac::algebra::TensorIndex>{}(idx);

		return value;
	}
};

}

namespace dirac {

namespace algebra {

namespace LI {

//----------------------------------------------------------------------

/**
 * If both indices are either upper or lower, returns metric.
 * For a lower and an upper index, returns Kronecker delta.
 */
template<typename Scalar>
TensorPolynomial<Scalar>
eta(const TensorIndex& mu, const TensorIndex& nu)  {
	std::string id = (mu.isUpper == nu.isUpper) ?
							Basis::eta : Basis::delta;
	return Tensor::create(id, Tensor::Indices{ mu, nu });
}

//----------------------------------------------------------------------

/**
 * Returns Levi-Civita symbol with specified indices.
 */
template<typename Scalar>
TensorPolynomial<Scalar> epsilon(const TensorIndex& kappa,
		const TensorIndex& lambda,
		const TensorIndex& mu,
		const TensorIndex& nu)  {
	return Tensor::create(Basis::epsilon,
			 Tensor::Indices{ kappa, lambda, mu, nu });
}

//----------------------------------------------------------------------

/**
 * Constructs an empty (pseudo)-tensor polynomial
 */
template<typename Scalar>
inline TensorPolynomial<Scalar> ZeroPoly() {
	return TensorPolynomial<Scalar>();
}

//----------------------------------------------------------------------

/**
 * Left multiplication of a (pseudo)-tensor polynomial
 * by a complex number
 */
template<typename Scalar>
inline TensorPolynomial<Scalar>
operator*(const Complex<Scalar>& c, const TensorPolynomial<Scalar>& p) {
	return prod<TensorPolynomial<Scalar>,
				Complex<Scalar>, Tensor>(c, p);
}

//----------------------------------------------------------------------

/**
 * Mutating left multiplication of a (pseudo)-tensor polynomial
 * by a basis (pseudo)-tensor
 */
template<typename Scalar>
inline TensorPolynomial<Scalar>&
operator*=(const Tensor& t, TensorPolynomial<Scalar>& p) {
	for (typename TensorPolynomial<Scalar>::Term& term : p.terms)
		term.factors.insert(term.factors.begin(), t);
	return p;
}

//----------------------------------------------------------------------

/**
 * Mutating left multiplication of a (pseudo)-tensor polynomial
 * by a complex number
 */
template<typename Scalar>
inline TensorPolynomial<Scalar>&
operator*=(const Complex<Scalar>& c, TensorPolynomial<Scalar>& p) {
	for (typename TensorPolynomial<Scalar>::Term& term : p.terms)
		term.coeff = c * term.coeff;
	return p;
}

//----------------------------------------------------------------------

/**
 * Negation operator for a basis Lorentz-invariant (pseudo)-tensor.
 * Results in a tensor polynomial.
 */
template<typename Scalar>
inline TensorPolynomial<Scalar> operator-(const Tensor& t) {
	TensorPolynomial<Scalar> res{t};
	res.terms[0].coeff = -one<Scalar>();
	return res;
}

//----------------------------------------------------------------------

template<typename Scalar>
void TensorPolynomial<Scalar>::canonicalize() {
	//Filter out zeros
	typename TensorPolynomial<Scalar>::Terms tmpTerms;
	for (typename TensorPolynomial<Scalar>::Term& term : this->terms)
		if (term.coeff != zero<Scalar>())
			tmpTerms.push_back(term);

	this->terms = tmpTerms;

	expandEpsilonPowers();
	contractIndices();
	mergeTerms();
}

//----------------------------------------------------------------------

template<typename Scalar>
void TensorPolynomial<Scalar>::expandEpsilonPowers() {
	typename TensorPolynomial<Scalar>::Terms tmpTerms;
	for (typename TensorPolynomial<Scalar>::Term& term : this->terms) {
		TensorPolynomial<Scalar> tmp{ term.coeff };
		std::optional<Tensor> epsCache;

		for (Tensor& factor : term.factors) {
			const std::string& aId = factor.id();
			if ((aId == Basis::delta) || (aId == Basis::eta)) {
				tmp *= factor;
			} else if (aId == Basis::epsilon) {
				if (!epsCache)
					epsCache = factor;
				else {
					Tensor& cached = epsCache.value();
					if (!factor.complete() || !cached.complete())
						throw std::runtime_error{
								"Levi-Civita symbol "
								"must have four indices" };

					TensorPolynomial<Scalar> expansion;
					forPermutations(4, [&](const Permutation& perm) {
						TensorPolynomial<Scalar>
						expTerm{ one<Scalar>() };

						const TensorIndices&
						factorIndices = factor.indices();

						const TensorIndices&
						cachedIndices = cached.indices();

						for (unsigned int i = 0; i < 4; ++i)
							expTerm *= eta<Scalar>(cachedIndices[i],
											factorIndices[perm.map[i]]);

						if (perm.isEven)
							expansion -= expTerm;
						else
							expansion += expTerm;
					});

					tmp *= expansion;
					epsCache.reset();
				}
			}
		}

		if (epsCache)
			tmp *= epsCache.value();

		tmpTerms.insert(tmpTerms.end(),
				tmp.terms.begin(), tmp.terms.end());
	}

	this->terms = tmpTerms;
}

//----------------------------------------------------------------------

template<typename Scalar>
void TensorPolynomial<Scalar>::contractIndices() {
	typename TensorPolynomial<Scalar>::Terms tmpTerms;
	tmpTerms.reserve(this->terms.size());

	for (typename TensorPolynomial<Scalar>::Term& term : this->terms) {
		std::optional<Term> tmp = contractIndices(term);
		if (tmp)
			tmpTerms.push_back(tmp.value());
	}

	this->terms = tmpTerms;
}

//----------------------------------------------------------------------

template<typename Scalar>
std::optional<typename TensorPolynomial<Scalar>::Term>
TensorPolynomial<Scalar>::contractIndices(
		const typename TensorPolynomial<Scalar>::Term& src) {
	if (src.coeff == zero<Scalar>())
		return std::optional<Term>{};

	if (src.factors.empty())
		return src;

	Term res{ src.coeff };

	std::vector<Tensor> metrics;
	metrics.reserve(src.factors.size());

	std::vector<Tensor> epsilons;
	epsilons.reserve(src.factors.size());

	for (const Tensor& factor : src.factors) {
		if (factor.id() ==  Basis::epsilon) {
			if (!factor.complete())
				throw std::runtime_error{factor.id()
											+ " requires four indices"};

			epsilons.push_back(factor);
		} else if ((factor.id() == Basis::eta)
				|| (factor.id() == Basis::delta)) {
			if (!factor.complete())
				throw std::runtime_error{factor.id()
											+ " requires two indices"};
			metrics.push_back(factor);
		} else
			throw std::runtime_error{
				"Invalid Lorentz-invariant tensor id" };
	}

	while (!metrics.empty()) {
		Tensor first = metrics[0];
		metrics.erase(metrics.begin());

		const TensorIndex& i1 = first.indices()[0];
		const TensorIndex& i2 = first.indices()[1];

		//Check for trace
		if (i1.dual(i2)) {
			res.coeff *= Complex<Scalar>{ 4, 0 };
			continue;
		}

		bool merged = false;

		//Try contracting with other metric tensors
		for (Tensor& m : metrics) {
			if (merged)
				continue;

			TensorIndices tmpIndices;
			tmpIndices.reserve(m.indices().size());
			for (const TensorIndex& idx: m.indices()) {
				if (idx.dual(i1) && !merged) {
					tmpIndices.push_back(i2);
					merged = true;
				} else if (idx.dual(i2) && !merged) {
					tmpIndices.push_back(i1);
					merged = true;
				} else {
					tmpIndices.push_back(idx);
				}
			}

			if (!merged)
				continue;

			bool hasUpper = false;
			bool hasLower = false;
			for (TensorIndex& idx: tmpIndices)
				if (idx.isUpper)
					hasUpper = true;
				else
					hasLower = true;

			std::string tensorId = (hasUpper && hasLower)
										? Basis::delta : Basis::eta;
			m = Tensor::create(tensorId, tmpIndices);
		}

		//Now try contracting with epsilons
		for (Tensor& eps : epsilons) {
			if (merged)
				continue;

			const TensorIndices& indices = eps.indices();
			for (size_t i = 0; i < indices.size(); ++i) {
				if (indices[i].dual(i1) && !merged) {
					eps.replaceIndex(i, i2);
					merged = true;
				} else if (indices[i].dual(i2) && !merged) {
					eps.replaceIndex(i, i1);
					merged = true;
				}
			}
		}

		if (!merged)
			res.factors.push_back(first);
	}

	for (Tensor& eps : epsilons) {
		const TensorIndices& indices = eps.indices();

		//Check for same indices in Livi-Civita symbol
		for (size_t i = 0; i < indices.size(); ++i)
			for (size_t j = i + 1; j < indices.size(); ++j)
				if (indices[i].dual(indices[j])
						|| (indices[i] == indices[j]))
					return std::optional<Term>{};

		res.factors.push_back(eps);
	}

	return res;
}

//----------------------------------------------------------------------

template<typename Scalar>
std::optional<typename TensorPolynomial<Scalar>::Term>
TensorPolynomial<Scalar>::tryMerge(const Term &t1, const Term &t2) {
	if (t1.factors.size() != t2.factors.size())
		return std::optional<Term>{};

	bool even = true;
	std::unordered_set<size_t> iMap1;
	iMap1.reserve(t1.factors.size());
	for (size_t i = 0; i < t1.factors.size(); ++i)
		iMap1.insert(i);

	std::unordered_set<size_t> iMap2;
	iMap2.reserve(t2.factors.size());
	for (size_t i = 0; i < t2.factors.size(); ++i)
		iMap2.insert(i);

	while (!iMap1.empty()) {
		size_t iFirst = *iMap1.begin();
		iMap1.erase(iFirst);

		std::optional<size_t> iMatch;
		for (size_t iSecond : iMap2) {
			if (iMatch)
				continue;

			const Tensor& first = t1.factors[iFirst];
			const Tensor& second = t2.factors[iSecond];

			std::optional<Permutation>
			mapping = first.mappingTo(second);
			if (mapping) {
				iMatch = iSecond;

				if (first.id() == Basis::epsilon) {
					even = (even && mapping.value().isEven)
							|| (!even && !mapping.value().isEven);
				}
			}
		}

		//If a non-matching factor is found, return empty optional
		if (iMatch)
			iMap2.erase(iMatch.value());
		else
			return std::optional<Term>{};
	}

	Term res{ t1 };
	if (even)
		res.coeff += t2.coeff;
	else
		res.coeff -= t2.coeff;

	return res;
}

//----------------------------------------------------------------------

template<typename Scalar>
void TensorPolynomial<Scalar>::mergeTerms(Merger merger) {
	typename TensorPolynomial<Scalar>::Terms tmpTerms;
	tmpTerms.reserve(this->terms.size());

	typename TensorPolynomial<Scalar>::Terms rest = this->terms;
	while (!rest.empty()) {
		Term first = rest[0];
		rest.erase(rest.begin());

		typename TensorPolynomial<Scalar>::Terms tmp;
		tmp.reserve(rest.size());
		for (Term& other: rest) {
			std::optional<Term> merged = merger(first, other);
			if (merged)
				first = merged.value();
			else
				tmp.push_back(other);
		}

		tmpTerms.push_back(first);
		rest = tmp;
	}

	this->terms = tmpTerms;
}

//----------------------------------------------------------------------

} /*namespace LI*/

} /*namespace algebra*/

} /*namespace dirac*/

#endif /* SRC_ALGEBRA_LORENTZINVARIANT_HPP_ */
