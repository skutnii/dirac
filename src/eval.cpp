/*
 * eval.cpp
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

using namespace symbolic;

static CanonicalExpr eval(const Operand& value) {
	if (std::holds_alternative<Complex>(value)) {
		CanonicalExpr res;
		res.coeffs(0) = std::get<Complex>(value);
		return res;
	}

	GammaPolynomial poly = getPoly(value);

	return reduceGamma(poly);
}

static CanonicalExpr eval(const OpList& ops) {
	if (ops.empty())
		throw std::runtime_error{ "Empty expression" };

	if (ops.size() == 1)
		return eval(ops.front());

	return eval(toProduct(ops));
}

CanonicalExpr eval(const std::string& expr) {
	StringInput input{ expr };
	Compiler compiler;
	compiler.compile(input);

	const Executable& opCode = compiler.opCode();
	Interpreter interpreter;
	interpreter.exec(opCode.begin(), opCode.end());

	const Interpreter::OpStack& stack = interpreter.stack();
	if (stack.size() != 1)
		throw std::runtime_error{ "Inconsistent expression" };

	return eval(stack.front());
}

}
