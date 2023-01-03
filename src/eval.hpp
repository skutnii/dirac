/*
 * eval.h
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#ifndef EVAL_H_
#define EVAL_H_

#include <string>
#include <ostream>
#include "Symbolic.hpp"
#include "Compiler.hpp"
#include "StringInput.hpp"
#include "Interpreter.hpp"


namespace dirac {

namespace symbolic {

template<typename Scalar>
CanonicalExpr<Scalar> eval(const Operand<Scalar>& value) {
	if (std::holds_alternative<Complex<Scalar>>(value)) {
		CanonicalExpr<Scalar> res;
		res.coeffs(0) = std::get<Complex<Scalar>>(value);
		return res;
	}

	GammaPolynomial<Scalar> poly = getPoly<Scalar>(value);

	return reduceGamma(poly);
}

template<typename Scalar>
CanonicalExpr<Scalar> eval(const OpList<Scalar>& ops) {
	if (ops.empty())
		throw std::runtime_error{ "Empty expression" };

	if (ops.size() == 1)
		return eval<Scalar>(ops.front());

	return eval<Scalar>(toProduct<Scalar>(ops));
}


/**
 * Main evaluation routine
 */
template<typename Scalar>
symbolic::CanonicalExpr<Scalar> eval(const std::string& expr) {
	StringInput<Scalar> input{ expr };
	Compiler<Scalar> compiler;
	compiler.compile(input);

	const Executable<Scalar>& opCode = compiler.opCode();
	Interpreter<Scalar> interpreter;
	interpreter.exec(opCode.begin(), opCode.end());

	const typename Interpreter<Scalar>::OpStack& stack = interpreter.stack();
	if (stack.size() != 1)
		throw std::runtime_error{ "Inconsistent expression" };

	return eval<Scalar>(stack.front());
}

}

}

#endif /* EVAL_H_ */
