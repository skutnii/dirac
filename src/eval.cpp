/*
 * compute.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#include "eval.hpp"

#include <stdexcept>

#include "Compiler.hpp"
#include "StringInput.hpp"

namespace dirac {

void eval(const std::string& expr, std::ostream& output) {

	StringInput input{ expr };
	try {
		Compiler compiler;
		compiler.compile(input);
		for (const auto& token: compiler.opCode())
			output << token << std::endl;
	} catch (std::exception& e) {
		output << e.what() << std::endl;
	}
}

}
