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

namespace symbolic {

template<typename Scalar>
using Operand = std::variant<Literal,
		algebra::Complex<Scalar>,
		Tensor,
		GammaPolynomial<Scalar> >;

template<typename Scalar>
using OpList = std::list<Operand<Scalar> >;

extern const Literal I;

template<typename Scalar>
Operand<Scalar> resolve(const Literal& literal) {
	if (literal == I)
		return algebra::I<Scalar>();

	return Tensor::create(literal);
}

template<typename Scalar>
GammaPolynomial<Scalar> getPoly(const Operand<Scalar>& op) {
	return std::holds_alternative<Tensor>(op) ?
			toPolynomial<Scalar>(std::get<Tensor>(op)) : std::get<GammaPolynomial<Scalar>>(op);
}

template<typename Scalar>
using BinaryOp = std::function<Operand<Scalar> (const Operand<Scalar>&, const Operand<Scalar>&)>;

template<typename Scalar>
OpList<Scalar> arithmeticBinary(const OpList<Scalar>& first,
		const OpList<Scalar>& second, BinaryOp<Scalar> op)  {
	if (first.empty() || second.empty())
		throw std::runtime_error{ "Empty binary operation argument" };

	if (first.size() > 1)
		return arithmeticBinary<Scalar>(toProduct<Scalar>(first), second, op);

	if (second.size() > 1)
		return arithmeticBinary<Scalar>(first, toProduct<Scalar>(second), op);

	OpList<Scalar> res;
	res.push_back(op(first.front(), second.front()));
	return res;
}

template<typename Scalar>
Operand<Scalar> sum(const Operand<Scalar>& op1, const Operand<Scalar>& op2) {
	if (std::holds_alternative<Literal>(op1))
		return sum<Scalar>(resolve<Scalar>(std::get<Literal>(op1)), op2);

	if (std::holds_alternative<Literal>(op2))
		return sum<Scalar>(op1, resolve<Scalar>(std::get<Literal>(op2)));

	if (std::holds_alternative<Complex<Scalar>>(op1)) {
		if (!std::holds_alternative<Complex<Scalar>>(op2))
			throw std::runtime_error{
				"Adding a number and non-numeric value is not allowed" };

		return std::get<Complex<Scalar>>(op1) + std::get<Complex<Scalar>>(op2);
	}

	return getPoly<Scalar>(op1) + getPoly<Scalar>(op2);
}

template<typename Scalar>
inline OpList<Scalar> sum(const OpList<Scalar>& first, const OpList<Scalar>& second) {
	return arithmeticBinary<Scalar>(first, second,
			[](const Operand<Scalar>& a, const Operand<Scalar>& b) -> Operand<Scalar> {
				return sum<Scalar>(a, b);
			});
}

template<typename Scalar>
Operand<Scalar> diff(const Operand<Scalar>& op1, const Operand<Scalar>& op2)  {
	if (std::holds_alternative<Literal>(op1))
		return diff(resolve<Scalar>(std::get<Literal>(op1)), op2);

	if (std::holds_alternative<Literal>(op2))
		return diff(op1, resolve<Scalar>(std::get<Literal>(op2)));

	if (std::holds_alternative<Complex<Scalar>>(op1)) {
		if (!std::holds_alternative<Complex<Scalar>>(op2))
			throw std::runtime_error{
				"Subtracting a number and non-numeric value is not allowed" };

		return std::get<Complex<Scalar>>(op1) - std::get<Complex<Scalar>>(op2);
	}

	return getPoly<Scalar>(op1) - getPoly<Scalar>(op2);
}

template<typename Scalar>
inline OpList<Scalar> diff(const OpList<Scalar>& first, const OpList<Scalar>& second) {
	return arithmeticBinary<Scalar>(first, second,
			[](const Operand<Scalar>& a, const Operand<Scalar>& b) -> Operand<Scalar> {
				return diff<Scalar>(a, b);
			});
}

template<typename Scalar>
OpList<Scalar> neg(const OpList<Scalar>& arg) {
	if (arg.empty())
		throw std::runtime_error{ "Empty negation argument" };

	if (arg.size() > 1)
		return neg<Scalar>(toProduct<Scalar>(arg));

	OpList<Scalar> res;
	res.push_back(neg<Scalar>(arg.front()));
	return res;
}

template<typename Scalar>
Operand<Scalar> neg(const Operand<Scalar>& arg) {
	if (std::holds_alternative<Literal>(arg))
		return neg<Scalar>(resolve<Scalar>(std::get<Literal>(arg)));

	if (std::holds_alternative<Complex<Scalar>>(arg))
		return -std::get<Complex<Scalar>>(arg);

	return -getPoly<Scalar>(arg);
}

template<typename Scalar>
Operand<Scalar> prod(const Operand<Scalar>& op1, const Operand<Scalar>& op2) {
	if (std::holds_alternative<Literal>(op1))
		return prod<Scalar>(resolve<Scalar>(std::get<Literal>(op1)), op2);

	if (std::holds_alternative<Literal>(op2))
		return prod<Scalar>(op1, resolve<Scalar>(std::get<Literal>(op2)));

	bool complexFirst = std::holds_alternative<Complex<Scalar>>(op1);
	bool complexSecond = std::holds_alternative<Complex<Scalar>>(op2);

	if (complexFirst && complexSecond)
		return std::get<Complex<Scalar>>(op1) * std::get<Complex<Scalar>>(op2);

	if (complexFirst && !complexSecond)
		return std::get<Complex<Scalar>>(op1) * getPoly<Scalar>(op2);

	if (!complexFirst && complexSecond)
		return getPoly<Scalar>(op1) * std::get<Complex<Scalar>>(op2);

	return getPoly<Scalar>(op1) * getPoly<Scalar>(op2);
}

template<typename Scalar>
inline OpList<Scalar> prod(const OpList<Scalar>& first, const OpList<Scalar>& second) {
	return arithmeticBinary<Scalar>(first, second,
			[](const Operand<Scalar>& a, const Operand<Scalar>& b) -> Operand<Scalar> {
				return prod<Scalar>(a, b);
			});
}

template<typename Scalar>
Operand<Scalar> div(const Operand<Scalar>& op1, const Operand<Scalar>& op2) {
	if (std::holds_alternative<Literal>(op1))
		return div<Scalar>(resolve<Scalar>(std::get<Literal>(op1)), op2);

	if (std::holds_alternative<Literal>(op2))
		return div<Scalar>(op1, resolve<Scalar>(std::get<Literal>(op2)));

	if (!std::holds_alternative<Complex<Scalar>>(op2))
		throw std::runtime_error{ "Can only divide by a number" };

	if (std::holds_alternative<Complex<Scalar>>(op1))
		return std::get<Complex<Scalar>>(op1) / std::get<Complex<Scalar>>(op2);

	return getPoly<Scalar>(op1) * ( algebra::one<Scalar>() / std::get<Complex<Scalar>>(op2));
}

template<typename Scalar>
inline OpList<Scalar> div(const OpList<Scalar>& first, const OpList<Scalar>& second) {
	return arithmeticBinary<Scalar>(first, second,
			[](const Operand<Scalar>& a, const Operand<Scalar>& b) -> Operand<Scalar> {
				return div<Scalar>(a, b);
			});

}

template<typename Scalar>
Tensor getTensor(const Operand<Scalar>& op) {
	if (std::holds_alternative<Tensor>(op))
		return std::get<Tensor>(op);

	if (!std::holds_alternative<Literal>(op))
		throw std::runtime_error{ "Can only convert a literal to a tensor" };

	return Tensor::create(std::get<Literal>(op));
}

template<typename Scalar>
TensorIndices toIndices(const OpList<Scalar>& list, bool isUpper) {
	TensorIndices indices;
	indices.reserve(list.size());
	for (const Operand<Scalar>& operand : list) {
		if (!std::holds_alternative<Literal>(operand))
			throw std::runtime_error{ "Can only convert a literal to a tensor index" };

		indices.emplace_back(std::get<Literal>(operand), isUpper);
	}

	return indices;
}

template<typename Scalar>
OpList<Scalar> subscript(const OpList<Scalar>& head, const OpList<Scalar>& indices) {
	if (head.empty() || (head.size() > 1))
		throw std::runtime_error{ "Subscript is only applicable to a single value" };

	OpList<Scalar> res;
	res.push_back(getTensor<Scalar>(head.front()) ^ toIndices<Scalar>(indices, false));
	return res;
}

template<typename Scalar>
OpList<Scalar> superscript(const OpList<Scalar>& head, const OpList<Scalar>& indices) {
	if (head.empty() || (head.size() > 1))
		throw std::runtime_error{ "Superscript is only applicable to a single value" };

	OpList<Scalar> res;
	res.push_back(getTensor<Scalar>(head.front()) ^ toIndices<Scalar>(indices, true));
	return res;
}

template<typename Scalar>
OpList<Scalar> toProduct(const OpList<Scalar>& ops) {
	if (ops.empty())
		return ops;

	Operand<Scalar> value = algebra::one<Scalar>();
	for (auto it = ops.begin(); it != ops.end(); ++it)
		value = prod(value, *it);

	OpList<Scalar> res;
	res.push_back(value);
	return res;
}

template<typename Scalar>
inline OpList<Scalar>
join(const OpList<Scalar>& list1, const OpList<Scalar>& list2) {
	OpList<Scalar> res;
	res.insert(res.end(), list1.begin(), list1.end());
	res.insert(res.end(), list2.begin(), list2.end());
	return res;
}

template<typename Scalar>
CanonicalExpr<Scalar> eval(const Operand<Scalar>& value) {
	using namespace symbolic;
	if (std::holds_alternative<Complex<Scalar>>(value)) {
		CanonicalExpr<Scalar> res;
		res.coeffs(0) = std::get<Complex<Scalar>>(value);
		return res;
	}

	GammaPolynomial<Scalar> poly = getPoly<Scalar>(value);

	return reduceGamma<Scalar>(poly);
}

template<typename Scalar>
CanonicalExpr<Scalar> eval(const OpList<Scalar>& ops) {
	if (ops.empty())
		throw std::runtime_error{ "Empty expression" };

	if (ops.size() == 1)
		return eval<Scalar>(ops.front());

	return eval<Scalar>(toProduct<Scalar>(ops));
}

}

}



#endif /* SRC_OPERATIONS_HPP_ */
