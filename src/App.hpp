/*
 * App.hpp
 *
 *  Created on: Jan 6, 2023
 *      Author: skutnii
 */

#ifndef SRC_APP_HPP_
#define SRC_APP_HPP_

#include <optional>
#include <string>
#include <limits>

#include "algebra/Gamma.hpp"
#include "ExprPrinter.hpp"
#include "utils.hpp"
#include "Compiler.hpp"
#include "Interpreter.hpp"
#include "StringInput.hpp"
#include "Operations.hpp"

namespace dirac {

/**
 * The App class encapsulates top-level command line app logic.
 */
class App {
public:
	/**
	 * Initializes app object with command line arguments
	 */
	App(int argc, char **argv);

	App() = default;
	App(const App& other) = delete;
	App(App&& other) = delete;
	App& operator=(const App& other) = delete;

	~App() = default;

	/**
	 * Set environment variable identified by first argument
	 * to value identified by second one.
	 * Recognized variables are
	 * 	- mode: arithmetic mode, "rational" for rational
	 * 		and "float" for floating point,
	 * 		default is rational;
	 * 	- line_terms: number of terms per line,
	 * 		"inf" or 0 meaning unlimited or integer expression,
	 * 		unlimited by default;
	 * 	- dummy: dummy index label, string, default is "\omega".
	 * 	- apply_symmetry: boolean, specifies whether the terms
	 * 						in the coefficient at \sigma^{\mu\nu}
	 * 						will be merged using antisymmetry of \sigma,
	 * 						default is true.
	 */
	void setVar(const std::string& name, const std::string& value);

	/**
	 * If expression was specified via command line,
	 * evaluates it and exits.
	 *
	 * Otherwise runs read-eval-print loop.
	 */
	int run();

	/**
	 * Parse mode string. Returns true for "float",
	 * false for "rational", empty optional otherwise.
	 */
	static std::optional<bool> getUseFloat(const std::string& str);

	/**
	 * Parse line terms count string.
	 * Allowed values are "inf" or integer constants.
	 * "inf" or 0 means no line breaks.
	 */
	static std::optional<size_t> getLineTerms(const std::string& str);

	/**
	 * Convert string to boolean. Returns true for "true",
	 * false for "false", empty optional otherwise.
	 */
	static std::optional<bool> getBoolean(const std::string& str);

	/**
	 * Process an expression and print the result to output.
	 * Template argument selects numeric type
	 * used in coefficients of tensor polynomials.
	 * compute<void> is resolved to compute<Rational> or compute<double>
	 * based on _useFloat value.
	 * Never throws; error messages are written to the output.
	 */
	template<typename Number>
	int compute(const std::string& input,
			std::ostream& output) const noexcept;

	/**
	 * Main evaluation routine.
	 * Parses an expression and converts it to canonical form.
	 * Throws std::runtime_error on malformed argument.
	 */
	template<typename Scalar>
	symbolic::CanonicalExpr<Scalar>
	compute(const std::string& expr) const;

private:
	/**
	 * Runs read-eval-print loop.
	 */
	int runShell();

	bool _useFloat = false;
	bool _applySymmetry = true;
	size_t _lineTerms = 0;
	std::string _commandLineExpr;
	std::string _dummyName = "\\omega";
};

//----------------------------------------------------------------------

template<typename Scalar>
symbolic::CanonicalExpr<Scalar>
App::compute(const std::string& expr) const {
	using namespace symbolic;

	StringInput<Scalar> input{ expr };
	Compiler<Scalar> compiler;
	compiler.compile(input);

	const Executable<Scalar>& opCode = compiler.opCode();
	Interpreter<Scalar> interpreter;
	interpreter.exec(opCode.begin(), opCode.end());

	const typename Interpreter<Scalar>::OpStack&
	stack = interpreter.stack();

	if (stack.size() != 1)
		throw std::runtime_error{ "Inconsistent expression" };

	CanonicalExpr<Scalar> res = eval<Scalar>(stack.front());
	if (_applySymmetry)
		res.applySymmetry();

	return res;
}

//----------------------------------------------------------------------

template<typename Number>
int App::compute(const std::string& input,
		std::ostream& output) const noexcept {
	using namespace symbolic;
	try {
		CanonicalExpr<Number> expr = compute<Number>(input);
		ExprPrinter<Number> printer{ _dummyName };
		output << printer.latexify(expr) << std::endl;
		return 0;
	} catch (std::exception& e) {
		output << e.what() << std::endl;
		return 1;
	}
}

//----------------------------------------------------------------------

template<>
int App::compute<void>(const std::string& input,
		std::ostream& output) const noexcept;

} /* namespace dirac */

#endif /* SRC_APP_HPP_ */
