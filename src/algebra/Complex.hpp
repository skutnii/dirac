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

using Complex = std::complex<double>;

inline Complex one() {
	return Complex{ 1, 0 };
}

inline Complex I() {
	return Complex{ 0, 1 };
}

inline Complex zero() {
	return Complex{ 0, 0 };
}

}

}



#endif /* SRC_ALGEBRA_COMPLEX_HPP_ */
