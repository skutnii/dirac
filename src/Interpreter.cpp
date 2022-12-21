/*
 * Processor.cpp
 *
 *  Created on: Dec 19, 2022
 *      Author: skutnii
 */

#include "Interpreter.hpp"

namespace dirac {

void Interpreter::process(const Token& token) {
	if (std::holds_alternative<Op>(token))
		process(std::get<Op>(token));
	else if (std::holds_alternative<Number>(token)) {
		OpList value;
		value.push_back(std::get<Number>(token));
		_stack.push_front(value);
	} else if (std::holds_alternative<Literal>(token))
		_stack.push_front(OpList{ std::get<Literal>(token) });
}

void Interpreter::process(const Op& op) {
	if (op == Op::Plus)
		performBinary([](const OpList& l1, const OpList& l2) -> OpList {
			return l1 + l2;
		});
	else if (op == Op::Minus)
		performBinary([](const OpList& l1, const OpList& l2) -> OpList {
			return l1 - l2;
		});
	else if (op == Op::UMinus)
		performUnary([](const OpList& l) -> OpList {
			return -l;
		});
	else if (op == Op::Mul)
		performBinary([](const OpList& l1, const OpList& l2) -> OpList {
			return l1 * l2;
		});
	else if (op == Op::Div)
		performBinary([](const OpList& l1, const OpList& l2) -> OpList {
			return l1 / l2;
		});
	else if (op == Op::Subs)
		performBinary(subscript);
	else if (op == Op::Super)
		performBinary(superscript);
	else if (op == Op::Splice)
		performBinary(join);
	else
		throw std::runtime_error{ "Unsupported operation: " + op.str() };
}

void Interpreter::performBinary(const BinaryOp &binOp) {
	if (_stack.size() < 2)
		throw std::runtime_error{ "Not enough arguments for a binary operation" };

	OpList& second = _stack[0];
	OpList& first = _stack[1];
	OpList res = binOp(first, second);
	_stack.pop_front();
	_stack.pop_front();
	_stack.push_front(res);
}

void Interpreter::performUnary(const UnaryOp &unaryOp) {
	if (_stack.empty())
		throw std::runtime_error{ "Not enough arguments for an unary operation" };

	OpList res = unaryOp(_stack.front());
	_stack.pop_front();
	_stack.push_front(res);
}

} /* namespace dirac */
