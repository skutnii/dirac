/*
 * compute.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#include "eval.h"
#include "Input.h"
#include <stdexcept>
#include "Compiler.h"

namespace dirac {

void eval(const std::string& expr, std::ostream& output) {
	Input input{ expr };
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
