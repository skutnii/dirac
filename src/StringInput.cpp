/*
 * Tokeniser.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#include "StringInput.hpp"

#include <cctype>
#include <stdexcept>

namespace dirac {

template<>
std::optional<double>
StringInput<double>::nextNumber() {
	std::string tmp;
	auto iter = _content.begin();
	while ((iter != _content.end())
			&& (isdigit(*iter) || *iter == '.')) {
		tmp += *iter;
		++iter;
	}

	skipTo(iter);
	return tmp.empty() ? std::optional<double>{} : std::stod(tmp);
}

template<>
std::optional<algebra::Rational>
StringInput<algebra::Rational>::nextNumber() {
	std::string tmp;
	auto iter = _content.begin();
	while ((iter != _content.end()) && isdigit(*iter)) {
		tmp += *iter;
		++iter;
	}

	skipTo(iter);
	return tmp.empty() ?
			std::optional<algebra::Rational>{}
				: algebra::Rational{ std::stoll(tmp) };
}

} /* namespace dirac */
