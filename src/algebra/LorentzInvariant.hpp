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

/**
 * If both indices are either upper or lower, returns metric.
 * For a lower and an upper index, returns Kronecker delta.
 */
Tensor eta(const TensorIndex& mu, const TensorIndex& nu);

/**
 * Returns Levi-Civita symbol with specified indices.
 */
Tensor epsilon(const TensorIndex& kappa,
		const TensorIndex& lambda,
		const TensorIndex& mu,
		const TensorIndex& nu);

struct TensorPolynomial : public Polynomial<Complex, Tensor> {
	using Coeff = Complex;
	using Base = Polynomial<Complex, Tensor>;

	TensorIndices indices;

	TensorPolynomial operator+(const TensorPolynomial& other) const {
		return sum<TensorPolynomial, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial operator-(const TensorPolynomial& other) const {
		return diff<TensorPolynomial, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial operator*(const TensorPolynomial& other) const {
		return prod<TensorPolynomial, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial operator*(const Coeff& c) const {
		return prod<TensorPolynomial, Coeff, Tensor>(*this, c);
	}

	TensorPolynomial() = default;
	TensorPolynomial(const TensorPolynomial& other) = default;
	TensorPolynomial(TensorPolynomial&& other) = default;
	TensorPolynomial& operator=(const TensorPolynomial& other) = default;

	TensorPolynomial(const Coeff& c)
		: Base{} {
		if ((c.real() != 0) || (c.imag() != 0))
			terms.emplace_back(c);
	}

	TensorPolynomial(const Tensor& t) : Base{} {
		terms.emplace_back(one(), t);
		indices = t.indices();
	}

	void canonicalize();
};

inline TensorPolynomial operator*(const Complex& c, const TensorPolynomial& p) {
	return prod<TensorPolynomial, Complex, Tensor>(c, p);
}

TensorPolynomial operator*(const Tensor& t1, const Tensor& t2);

inline TensorPolynomial operator-(const Tensor& t) {
	TensorPolynomial res{t};
	res.terms[0].coeff = -one();
	return res;
}

} /*namespace LI*/

template<>
inline void canonicalize<LI::TensorPolynomial,
	Complex, LI::Tensor>(LI::TensorPolynomial& tp) {
	tp.canonicalize();
}

}

}


#endif /* SRC_ALGEBRA_LORENTZINVARIANT_HPP_ */
