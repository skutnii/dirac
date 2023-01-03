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

} /* namespace symbolic */

} /* namespace dirac */
