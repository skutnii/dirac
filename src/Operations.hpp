/*
 * Operand.hpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */

#ifndef SRC_OPERATIONS_HPP_
#define SRC_OPERATIONS_HPP_

#include "Token.hpp"
#include "Symbolic.hpp"
#include <list>
#include <string>
#include <functional>

namespace dirac {

using Operand = std::variant<Literal,
		algebra::Complex,
		symbolic::Tensor,
		symbolic::GammaPolynomial>;

using OpList = std::list<Operand>;

Operand resolve(const Literal& l);

symbolic::GammaPolynomial getPoly(const Operand& op);

using BinaryOp = std::function<Operand (const Operand&, const Operand&)>;

OpList arithmeticBinary(const OpList& first, const OpList& second, BinaryOp op);

Operand sum(const Operand& op1, const Operand& op2);

inline OpList operator+(const OpList& first, const OpList& second) {
	return arithmeticBinary(first, second, sum);
}

Operand diff(const Operand& op1, const Operand& op2);

inline OpList operator-(const OpList& first, const OpList& second) {
	return arithmeticBinary(first, second, diff);
}

OpList operator-(const OpList& arg);
Operand operator-(const Operand& arg);

Operand prod(const Operand& op1, const Operand& op2);

inline OpList operator*(const OpList& first, const OpList& second) {
	return arithmeticBinary(first, second, prod);
}

Operand div(const Operand& op1, const Operand& op2);

inline OpList operator/(const OpList& first, const OpList& second) {
	return arithmeticBinary(first, second, div);
}

symbolic::Tensor getTensor(const Operand& op);
symbolic::TensorIndices toIndices(const OpList& list, bool isUpper);

OpList subscript(const OpList& head, const OpList& indices);
OpList superscript(const OpList& head, const OpList& indices);

OpList toProduct(const OpList& ops);

inline OpList join(const OpList& list1, const OpList& list2) {
	OpList res;
	res.insert(res.end(), list1.begin(), list1.end());
	res.insert(res.end(), list2.begin(), list2.end());
	return res;
}

}



#endif /* SRC_OPERATIONS_HPP_ */
