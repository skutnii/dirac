/*
 * Operations.cpp
 *
 *  Created on: Dec 21, 2022
 *      Author: skutnii
 */

#include "Operations.hpp"
#include "Symbolic.hpp"
#include <variant>

namespace dirac {

static const Literal I = "\\I";

using namespace symbolic;
using Complex = algebra::Complex;

OpList arithmeticBinary(const OpList& first, const OpList& second, BinaryOp op) {
	if (first.empty() || second.empty())
		throw std::runtime_error{ "Empty binary operation argument" };

	if (first.size() > 1)
		return arithmeticBinary(toProduct(first), second, op);

	if (second.size() > 1)
		return arithmeticBinary(first, toProduct(second), op);

	OpList res;
	res.push_back(op(first.front(), second.front()));
	return res;
}

Operand resolve(const Literal& literal) {
	if (literal == I)
		return algebra::I();

	return Tensor::create(literal);
}

GammaPolynomial getPoly(const Operand &op) {
	return std::holds_alternative<Tensor>(op) ?
			toPolynomial(std::get<Tensor>(op)) : std::get<GammaPolynomial>(op);
}

Operand sum(const Operand& op1, const Operand& op2) {
	if (std::holds_alternative<Literal>(op1))
		return sum(resolve(std::get<Literal>(op1)), op2);

	if (std::holds_alternative<Literal>(op2))
		return sum(op1, resolve(std::get<Literal>(op2)));

	if (std::holds_alternative<Complex>(op1)) {
		if (!std::holds_alternative<Complex>(op2))
			throw std::runtime_error{
				"Adding a number and non-numeric value is not allowed" };

		return std::get<Complex>(op1) + std::get<Complex>(op2);
	}

	return getPoly(op1) + getPoly(op2);
}

Operand diff(const Operand& op1, const Operand& op2) {
	if (std::holds_alternative<Literal>(op1))
		return diff(resolve(std::get<Literal>(op1)), op2);

	if (std::holds_alternative<Literal>(op2))
		return diff(op1, resolve(std::get<Literal>(op2)));

	if (std::holds_alternative<Complex>(op1)) {
		if (!std::holds_alternative<Complex>(op2))
			throw std::runtime_error{
				"Subtracting a number and non-numeric value is not allowed" };

		return std::get<Complex>(op1) - std::get<Complex>(op2);
	}

	return getPoly(op1) - getPoly(op2);
}

OpList operator-(const OpList& arg) {
	if (arg.empty())
		throw std::runtime_error{ "Empty negation argument" };

	if (arg.size() > 1)
		return -toProduct(arg);

	OpList res;
	res.push_back(-arg.front());
	return res;
}

Operand operator-(const Operand& op) {
	if (std::holds_alternative<Literal>(op))
		return -resolve(std::get<Literal>(op));

	if (std::holds_alternative<Complex>(op))
		return -std::get<Complex>(op);

	return -getPoly(op);
}

Operand prod(const Operand& op1, const Operand& op2) {
	if (std::holds_alternative<Literal>(op1))
		return prod(resolve(std::get<Literal>(op1)), op2);

	if (std::holds_alternative<Literal>(op2))
		return prod(op1, resolve(std::get<Literal>(op2)));

	bool complexFirst = std::holds_alternative<Complex>(op1);
	bool complexSecond = std::holds_alternative<Complex>(op2);

	if (complexFirst && complexSecond)
		return std::get<Complex>(op1) * std::get<Complex>(op2);

	if (complexFirst && !complexSecond)
		return std::get<Complex>(op1) * getPoly(op2);

	if (!complexFirst && complexSecond)
		return getPoly(op1) * std::get<Complex>(op2);

	return getPoly(op1) * getPoly(op2);
}

Operand div(const Operand& op1, const Operand& op2) {
	if (!std::holds_alternative<Complex>(op2))
		throw std::runtime_error{ "Can only divide by a number" };

	if (std::holds_alternative<Complex>(op1))
		return std::get<Complex>(op1) / std::get<Complex>(op2);

	return getPoly(op1) * ( algebra::one() / std::get<Complex>(op2));
}

OpList toProduct(const OpList& ops) {
	if (ops.empty())
		return ops;

	Operand value = algebra::one();
	for (auto it = ops.begin(); it != ops.end(); ++it)
		value = prod(value, *it);

	OpList res;
	res.push_back(value);
	return res;
}

Tensor getTensor(const Operand &op) {
	if (std::holds_alternative<Tensor>(op))
		return std::get<Tensor>(op);

	if (!std::holds_alternative<Literal>(op))
		throw std::runtime_error{ "Can only convert a literal to a tensor" };

	return Tensor::create(std::get<Literal>(op));
}

OpList subscript(const OpList &head, const OpList &indices) {
	if (head.empty() || (head.size() > 1))
		throw std::runtime_error{ "Subscript is only applicable to a single value" };

	OpList res;
	res.push_back(getTensor(head.front()) ^ toIndices(indices, false));
	return res;
}

OpList superscript(const OpList &head, const OpList &indices) {
	if (head.empty() || (head.size() > 1))
		throw std::runtime_error{ "Superscript is only applicable to a single value" };

	OpList res;
	res.push_back(getTensor(head.front()) ^ toIndices(indices, true));
	return res;
}

TensorIndices toIndices(const OpList& list, bool isUpper) {
	TensorIndices indices;
	indices.reserve(list.size());
	for (const Operand& operand : list) {
		if (!std::holds_alternative<Literal>(operand))
			throw std::runtime_error{ "Can only convert a literal to a tensor index" };

		indices.emplace_back(std::get<Literal>(operand), isUpper);
	}

	return indices;
}

}


