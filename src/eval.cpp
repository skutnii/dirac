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
#include "Interpreter.hpp"

namespace dirac {

void eval(const std::string& expr, std::ostream& output) {
	using namespace symbolic;

	StringInput input{ expr };
	try {
		Compiler compiler;
		compiler.compile(input);

		const Executable& opCode = compiler.opCode();
		Interpreter interpreter;
		interpreter.exec(opCode.begin(), opCode.end());

		const Interpreter::OpStack& stack = interpreter.stack();
		if ((stack.size() != 1) ||
				(stack.front().size() != 1))
			throw std::runtime_error{ "Inconsistent expression" };

		const Operand& value = stack.front().front();
		if (std::holds_alternative<Complex>(value))
			output << std::get<Complex>(value);
		else if (std::holds_alternative<GammaPolynomial>(value))
			output << reduceGamma(std::get<GammaPolynomial>(value));
		else
			throw std::runtime_error{ "Inconsistent expression" };
	} catch (std::exception& e) {
		output << e.what() << std::endl;
	}
}

}
