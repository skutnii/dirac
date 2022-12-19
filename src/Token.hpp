/*
 * Token.h
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

	bool operator==(const Op& other) const {
		return (_repr == other._repr);
	}

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

inline std::ostream& operator<<(std::ostream& os, const Op& op) {
	return (os << op.str());
}

using Number = double;
using Token = std::variant<Op, Number, std::string>;

std::ostream& operator<<(std::ostream& os, const Token& token);

inline bool isOp(const Token& token) {
	return std::holds_alternative<Op>(token);
}

inline bool isValue(const Token& token) {
	return !isOp(token);
}

} /* namespace dirac */

#endif /* SRC_TOKEN_HPP_ */
