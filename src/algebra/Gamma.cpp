/*
 * Gamma.cpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */


#include "GammaMatrix.hpp"
#include "Gamma.hpp"

namespace dirac {

namespace algebra {

const std::string GammaBasis::gamma = "\\gamma";
const std::string GammaBasis::sigma = "\\sigma";
const std::string GammaBasis::gamma5 = "\\gamma5";

const GammaBasis::NameSet GammaBasis::Elements{
	GammaBasis::gamma,
	GammaBasis::gamma5,
	GammaBasis::sigma
};

}

}
