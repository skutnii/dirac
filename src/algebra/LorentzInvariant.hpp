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

namespace dirac {

namespace algebra {

//Lorentz invariant tensors namespace
namespace LI {

struct TensorBasis {
	TensorBasis() = default;

	static const std::string eta;
	static const std::string epsilon;
	static const std::string delta;

	using NameSet = std::unordered_set<std::string>;
	static const NameSet Elements;

	inline static bool allows(const std::string& id) {
		return (Elements.find(id) != Elements.end());
	}
};

} /* namespace LI*/

template<>
class TensorBase<std::string, LI::TensorBasis, std::string>
: public Tensorial {
public:

	static size_t maxIndexCount(const std::string& id) {
		if (id == LI::TensorBasis::epsilon)
			return 4;

		if ((id == LI::TensorBasis::delta)
			|| (id == LI::TensorBasis::eta))
			return 2;

		return 0;
	}
};

namespace LI {

using Tensor = TensorBase<std::string, TensorBasis, std::string>;
using Complex = std::complex<double>;

struct TensorPolynomial : public Polynomial<Complex, Tensor> {
	using Coeff = Complex;

	Tensorial metaTensor;

	TensorPolynomial operator+(const TensorPolynomial& other) const {
		return sum<TensorPolynomial, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial operator-(const TensorPolynomial& other) const {
		return diff<TensorPolynomial, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial operator*(const TensorPolynomial& other) const {
		return prod<TensorPolynomial, Coeff, Tensor>(*this, other);
	}

	TensorPolynomial() = default;
	TensorPolynomial(const TensorPolynomial& other) = default;
	TensorPolynomial(TensorPolynomial&& other) = default;
	TensorPolynomial& operator=(const TensorPolynomial& other) = default;

	TensorPolynomial(const Coeff& c)
		: Polynomial<std::complex<double>, Tensor>{} {
		if ((c.real() != 0) || (c.imag() != 0))
			terms.emplace_back(c);
	}

	void canonicalize();
};

} /*namespace LI*/

template<>
inline void canonicalize<LI::TensorPolynomial,
	LI::Complex, LI::Tensor>(LI::TensorPolynomial& tp) {
	tp.canonicalize();
}

}

}


#endif /* SRC_ALGEBRA_LORENTZINVARIANT_HPP_ */
