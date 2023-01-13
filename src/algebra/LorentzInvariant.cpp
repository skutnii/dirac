/*
 * LorentzInvariant.cpp
 *
 * Lorentz-invariant (pseudo)-tensor basis element name constants.
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

const std::string Basis::eta{ "\\eta" };
const std::string Basis::epsilon{ "\\epsilon" };
const std::string Basis::delta{ "\\delta" };

const Basis::NameSet Basis::Elements {
	Basis::eta,
	Basis::delta,
	Basis::epsilon
};

} /* namespace LI */

} //Namespace algebra

} //namespace dirac


