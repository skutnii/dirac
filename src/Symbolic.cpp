/*
 * Symbolic.cpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */


#include "Symbolic.hpp"

#include "algebra/GammaMatrix.hpp"

namespace dirac {

namespace symbolic {

const std::string Basis::gamma = "\\gamma";
const std::string Basis::sigma = "\\sigma";
const std::string Basis::gamma5 = "\\gamma5";

const Basis::NameSet Basis::Elements{
	Basis::gamma,
	Basis::gamma5,
	Basis::sigma
};

}

}
