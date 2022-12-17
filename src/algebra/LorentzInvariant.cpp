/*
 * Tensor.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#include <stdexcept>
#include <algorithm>
#include "LorentzInvariant.hpp"

namespace dirac {

namespace algebra {

namespace LI {

const std::string TensorBasis::eta{ "\\eta" };
const std::string TensorBasis::epsilon{ "\\epsilon" };
const std::string TensorBasis::delta{ "\\delta" };

const TensorBasis::NameSet TensorBasis::Elements {
	TensorBasis::eta,
	TensorBasis::delta,
	TensorBasis::epsilon
};

void TensorPolynomial::canonicalize() {

}

} /* namespace LI */

} //Namespace algebra

} //namespace dirac


