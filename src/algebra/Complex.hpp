/*
 * Complex.hpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */

#ifndef SRC_ALGEBRA_COMPLEX_HPP_
#define SRC_ALGEBRA_COMPLEX_HPP_

namespace dirac {

namespace algebra {

template<typename Scalar>
using Complex = std::complex<Scalar>;

template<typename Scalar>
inline Complex<Scalar> one() {
	return Complex<Scalar>{ 1, 0 };
}

template<typename Scalar>
inline Complex<Scalar> I() {
	return Complex<Scalar>{ 0, 1 };
}

template<typename Scalar>
inline Complex<Scalar> zero() {
	return Complex<Scalar>{ 0, 0 };
}

}

}



#endif /* SRC_ALGEBRA_COMPLEX_HPP_ */
