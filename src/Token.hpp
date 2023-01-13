/*
 * Token.h
 *
 * Expression token definition.
 * Token is the basic unit of a dirac expression:
 * - a number;
 * - a literal;
 * - an operation;
 * - a bracket.
 *
 *  Created on: Dec 12, 2022
 *      Author: skutnii
 */

#ifndef SRC_TOKEN_HPP_
#define SRC_TOKEN_HPP_

#include <variant>
#include <string>
#include <complex>
#include <ostream>

namespace dirac {

/**
 * Operations. Brackets are considered operations as well
 */
class Op {
public:
	static const Op Nop; //Not an operation
	static const Op Plus; // +
	static const Op Minus; // binary -
	static const Op UMinus; // unary -
	static const Op Mul; // *
	static const Op Div; // /
	static const Op LBrace; // {
	static const Op RBrace; // }
	static const Op Subs; // subscript _
	static const Op Super; // superscript ^
	static const Op Splice; // list concatenation

	Op() = default;
	Op(const Op& other) = default;

	/**
	 * Constructs an operation fro a symbol
	 */
	explicit Op(char c) {
		switch (c) {
		case '+':
		case '-':
		case '*':
		case '/':
		case '{':
		case '}':
		case '_':
		case '^':
		case '&':
			_repr = c;
			break;
		default:
			break;
		}
	}

	/**
	 * Equality check
	 */
	bool operator==(const Op& other) const {
		return (_repr == other._repr);
	}

	/**
	 * String representation
	 */
	std::string str() const  {
		if (*this == Nop)
			return "NOP";
		else if (*this == UMinus)
			return "-";

		return std::string{ _repr };
	}

private:
	char _repr = 0;
};

//----------------------------------------------------------------------

/**
 * Token printing
 */
inline std::ostream& operator<<(std::ostream& os, const Op& op) {
	return (os << op.str());
}

//----------------------------------------------------------------------

/**
 * Literal definition
 */
using Literal = std::string;

//----------------------------------------------------------------------

/**
 * Token definition
 */
template<typename Number>
using Token = std::variant<Op, Number, Literal>;

//----------------------------------------------------------------------

/**
 * Checks whether the argument represents an operation
 */
template<typename Number>
inline bool isOp(const Token<Number>& token) {
	return std::holds_alternative<Op>(token);
}

//----------------------------------------------------------------------

/**
 * Checks whether the argument represents a value
 * (that is, not an operation)
 */
template<typename Number>
inline bool isValue(const Token<Number>& token) {
	return !isOp(token);
}

} /* namespace dirac */

#endif /* SRC_TOKEN_HPP_ */
