/*
 * ExprPrinter.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: skutnii
 */

#include "ExprPrinter.hpp"
#include <string>
#include <sstream>

namespace dirac {

namespace symbolic {

using namespace algebra;

const std::string leftBrace{ "{" };
const std::string rightBrace{ "}" };
const std::string hat{ "^" };
const std::string subs{ "_" };
const std::string leftSquareBracket{ "\\left[" };
const std::string rightSquareBracket{ "\\right]" };
const std::string leftBracket{ "\\left(" };
const std::string rightBracket{ "\\right)" };

template<>
std::string
ExprPrinter<algebra::Rational>::latexify(const algebra::Rational& r) {
	std::stringstream ss;
	if (r.num() < 0)
		ss << "-";
	ss << "\\frac{" << r.absNum() << "}{" << r.den() << "}";
	return ss.str();
}

template<>
std::string ExprPrinter<Rational>::latexify(const Complex<Rational>& c) {
	bool hasReal = (c.real() != 0);
	bool hasImag = (c.imag() != 0);

	std::string value;
	if (hasReal)
		value += latexify(c.real());

	if (hasReal && (c.imag() > 0))
		value += " + ";
	else if (c.imag() < 0)
		value += " - ";

	if (hasImag) {
		value += "\\frac{";
		unsigned long long absNum = c.imag().absNum();
		if (absNum != 1)
			value += std::to_string(absNum);
		value += std::string{ "I}{" } + std::to_string(c.imag().den()) + "}";
	}

	return value;
}

} /* namespace symbolic */

} /* namespace dirac */
