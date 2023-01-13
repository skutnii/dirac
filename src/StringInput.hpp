/*
 * StringInput.hpp
 *
 * Basic string parsing
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <string>
#include <unordered_set>

#include "InputSequence.hpp"
#include "Token.hpp"
#include "algebra/Rational.hpp"

namespace dirac {

/**
 * Splits input into meaningful fragments
 */
template<typename Number>
class StringInput {
public:
	/**
	 * Constructs a string input object from a string.
	 * The string is not copied. Internally, StringInput
	 * instance keeps an std::string_view object.
	 */
	StringInput(const std::string& src)
		: _content{src.begin(), src.end()} {};

	/**
	 * Returns next token in the string if possible
	 */
	std::optional<Token<Number>> nextToken();

	/**
	 * Checks if theree are any more characters in the input
	 */
	bool empty() const { return _content.empty(); }

	virtual ~StringInput() = default;

	using CharSet = std::unordered_set<char>;

private:
	/**
	 * Skips whitespace
	 */
	void skipSpace() {
		auto iter = _content.begin();
		while ((iter != _content.end()) && isspace(*iter))
			++iter;

		skipTo(iter);
	}

	/**
	 * Tries reading a number from the input.
	 * Default implementation returns an empty optional;
	 * specializations for double and algebra::Rational
	 * implement number parsing logic.
	 */
	std::optional<Number> nextNumber() {
		return std::optional<Number>{};
	}

	/**
	 * Returns next symbol (that is, "\"
	 * followed by an alphanumeric sequence).
	 */
	std::string nextSymbol();

	/**
	 * Skip processed characters up to the argument
	 */
	void skipTo(std::string_view::iterator& it) {
		_content = std::string_view{ it, _content.end() };
	}

	/**
	 * Skip processed characters up to the argument
	 */
	void skipTo(size_t pos) {
		_content = _content.substr(pos);
	}

	std::string_view _content;
};

//----------------------------------------------------------------------

/**
 * Real number parsing
 */
template<>
std::optional<double>
StringInput<double>::nextNumber();

//----------------------------------------------------------------------

/**
 * Rational number parsing.
 * Actually, rationals are not parsed directly,
 * this method scans for integers and converts them to rationals.
 */
template<>
std::optional<algebra::Rational>
StringInput<algebra::Rational>::nextNumber();

//----------------------------------------------------------------------

template<typename Number>
std::optional<Token<Number>>
StringInput<Number>::nextToken() {
	if (_content.empty())
		return std::optional<Token<Number>>{};

	skipSpace();

	char next = _content[0];

	//Check operation
	Op op{next};
	if (op != Op::Nop) {
		skipTo(1);
		return op;
	}

	//Number encountered
	std::optional<Number> num = nextNumber();
	if (num)
		return num.value();

	if (next == '\\')
		return nextSymbol();

	if (isalpha(next))
		throw std::runtime_error(std::string{ _content }
				+ ": symbols must start with \"\\\"");

	throw std::runtime_error{std::string{ _content }
			+ ": unrecognized character \""
			+ std::string{ next } + "\""};

	return std::optional<Token<Number>>{};
}

//----------------------------------------------------------------------

template<typename Number>
std::string StringInput<Number>::nextSymbol() {
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

//----------------------------------------------------------------------

} /* namespace dirac */

#endif /* INPUT_H_ */
