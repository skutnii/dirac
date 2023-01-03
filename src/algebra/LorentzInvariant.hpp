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

namespace dirac {

namespace algebra {

//Lorentz invariant tensors namespace
namespace LI {

struct Basis {
	Basis() = default;

	static const std::string eta;
	static const std::string epsilon;
	static const std::string delta;

	using NameSet = std::unordered_set<std::string>;
	static const NameSet Elements;

	inline static bool allows(const std::string& id) {
		return (Elements.find(id) != Elements.end());
	}

	inline static size_t maxIndexCount(const std::string& id) {
		if (id == epsilon)
			return 4;

		if ((id == delta)
			|| (id == eta))
			return 2;

		return 0;
	}

};

} /* namespace LI*/

namespace LI {

using Tensor = TensorBase<std::string, Basis, IndexId>;

template<typename Scalar>
struct TensorPolynomial : public Polynomial<Complex<Scalar>, Tensor> {
	using Coeff = Complex<Scalar>;
	using Base = Polynomial<Complex<Scalar>, Tensor>;
	using Term = typename Base::Term;

	TensorIndices indices;

	TensorPolynomial<Scalar>
	operator+(const TensorPolynomial<Scalar>& other) const {
		return sum<TensorPolynomial<Scalar>, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial<Scalar>
	operator-(const TensorPolynomial<Scalar>& other) const {
		return diff<TensorPolynomial<Scalar>, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial<Scalar>
	operator*(const TensorPolynomial<Scalar>& other) const {
		return prod<TensorPolynomial<Scalar>, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial<Scalar> operator*(const Coeff& c) const {
		return prod<TensorPolynomial<Scalar>, Coeff, Tensor>(*this, c);
	}

	TensorPolynomial<Scalar> operator-() const {
		return negate<TensorPolynomial<Scalar>, Coeff, Tensor>(*this);
	}

	TensorPolynomial<Scalar>& operator+=(const TensorPolynomial<Scalar>& other) {
		return add<TensorPolynomial<Scalar>, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial<Scalar>& operator-=(const TensorPolynomial<Scalar>& other) {
		return sub<TensorPolynomial<Scalar>, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial<Scalar>& operator*=(const TensorPolynomial<Scalar>& other) {
		this->terms = (*this * other).terms;
		return *this;
	}

	TensorPolynomial<Scalar>& operator*=(const Tensor& t) {
		for (Term& term : this->terms)
			term.factors.push_back(t);

		return *this;
	}

	TensorPolynomial<Scalar>& operator*=(const Coeff& c) {
		for (Term& term : this->terms)
			term.coeff = term.coeff * c;

		return *this;
	}

	TensorPolynomial() = default;
	TensorPolynomial(const TensorPolynomial<Scalar>& other) = default;
	TensorPolynomial(TensorPolynomial<Scalar>&& other) = default;
	TensorPolynomial& operator=(const TensorPolynomial<Scalar>& other) = default;

	TensorPolynomial(const Coeff& c)
		: Base{} {
		if ((c.real() != 0) || (c.imag() != 0))
			this->terms.emplace_back(c);
	}

	TensorPolynomial(const Scalar& s) : TensorPolynomial{ Coeff{ s, static_cast<Scalar>(0) } } {}

	TensorPolynomial(const Tensor& t) : Base{} {
		this->terms.emplace_back(one<Scalar>(), t);
		indices = t.indices();
	}

	void canonicalize() override;

	bool isZero() const {
		if (this->terms.empty())
			return true;

		for (const Term& term : this->terms)
			if (term.coeff != zero<Scalar>())
				return false;

		return true;
	}

	void expandEpsilonPowers();
	void contractMetric();
	void contractKronecker();
	void mergeTerms();
};

/**
 * If both indices are either upper or lower, returns metric.
 * For a lower and an upper index, returns Kronecker delta.
 */
template<typename Scalar>
TensorPolynomial<Scalar> eta(const TensorIndex& mu, const TensorIndex& nu)  {
	std::string id = (mu.isUpper == nu.isUpper) ?
			Basis::eta : Basis::delta;
	return Tensor::create(id, Tensor::Indices{ mu, nu });
}

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

template<typename Scalar>
inline TensorPolynomial<Scalar> ZeroPoly() {
	return TensorPolynomial<Scalar>();
}

template<typename Scalar>
inline TensorPolynomial<Scalar> operator*(const Complex<Scalar>& c, const TensorPolynomial<Scalar>& p) {
	return prod<TensorPolynomial<Scalar>, Complex<Scalar>, Tensor>(c, p);
}

template<typename Scalar>
inline TensorPolynomial<Scalar>& operator*=(const Tensor& t, TensorPolynomial<Scalar>& p) {
	for (typename TensorPolynomial<Scalar>::Term& term : p.terms)
		term.factors.insert(term.factors.begin(), t);
	return p;
}

template<typename Scalar>
inline TensorPolynomial<Scalar>& operator*=(const Complex<Scalar>& c, TensorPolynomial<Scalar>& p) {
	for (typename TensorPolynomial<Scalar>::Term& term : p.terms)
		term.coeff = c * term.coeff;
	return p;
}

template<typename Scalar>
TensorPolynomial<Scalar> operator*(const Tensor& t1, const Tensor& t2) {
	TensorPolynomial<Scalar> res;
	res.terms.emplace_back(one<Scalar>());
	typename TensorPolynomial<Scalar>::Term& term = res.terms.back();
	term.factors.push_back(t1);
	term.factors.push_back(t2);

	res.canonicalize();
	return res;
}

template<typename Scalar>
inline TensorPolynomial<Scalar> operator-(const Tensor& t) {
	TensorPolynomial<Scalar> res{t};
	res.terms[0].coeff = -one<Scalar>();
	return res;
}

template<typename Scalar>
void TensorPolynomial<Scalar>::canonicalize() {
	expandEpsilonPowers();
	contractMetric();
	contractKronecker();
	mergeTerms();
}

template<typename Scalar>
void TensorPolynomial<Scalar>::expandEpsilonPowers() {
	//TODO: implement!
}

template<typename Scalar>
void TensorPolynomial<Scalar>::contractMetric() {
	//TODO: implement!
}

template<typename Scalar>
void TensorPolynomial<Scalar>::contractKronecker() {
	//TODO: implement!
}

template<typename Scalar>
void TensorPolynomial<Scalar>::mergeTerms() {
	//TODO: implement!
}

} /*namespace LI*/

}

}


#endif /* SRC_ALGEBRA_LORENTZINVARIANT_HPP_ */
