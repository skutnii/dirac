/*
 * Tokeniser.cpp
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#include "StringInput.h"

#include <cctype>
#include <stdexcept>

namespace dirac {

std::optional<Token> StringInput::nextToken() {
	if (_content.empty())
		return std::optional<Token>{};

	skipSpace();

	char next = _content[0];

	//Check operation
	Op op{next};
	if (op != Op::Nop) {
		skipTo(1);
		return op;
	}

	//Number encountered
	if (isdigit(next) || next == '.')
		return nextNumber();

	if (next == '\\')
		return nextSymbol();

	if (isalpha(next))
		throw std::runtime_error(std::string{ _content }
				+ ": symbols must start with \"\\\"");

	throw std::runtime_error{std::string{ _content }
			+ ": unrecognized character \"" + std::string{ next } + "\""};

	return std::optional<Token>{};
}

void StringInput::skipSpace() {
	auto iter = _content.begin();
	while ((iter != _content.end()) && isspace(*iter))
		++iter;

	skipTo(iter);
}

Number StringInput::nextNumber() {
	std::string tmp;
	auto iter = _content.begin();
	while ((iter != _content.end())
			&& (isdigit(*iter) || *iter == '.')) {
		tmp += *iter;
		++iter;
	}

	skipTo(iter);
	return std::stod(tmp);
}

std::string StringInput::nextSymbol() {
	std::string out;
	out += _content[0];
	auto iter = _content.begin() + 1;
	while ((iter != _content.end())
			&& (isalpha(*iter) || isdigit(*iter))) {
		out += *iter;
		++iter;
	}

	skipTo(iter);
	return out;
}

void StringInput::skipTo(std::string_view::iterator& it) {
	_content = std::string_view{ it, _content.end() };
}

void StringInput::skipTo(size_t pos) {
	_content = _content.substr(pos);
}

} /* namespace dirac */
