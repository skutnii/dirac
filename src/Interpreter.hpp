/*
 * Interperter.hpp
 *
 * Executable interpreter class
 *
 *  Created on: Dec 19, 2022
 *      Author: skutnii
 */

#ifndef SRC_INTERPRETER_HPP_
#define SRC_INTERPRETER_HPP_

#include "Token.hpp"
#include <variant>
#include <deque>
#include <list>
#include <iterator>
#include <optional>
#include <functional>

#include "algebra/Gamma.hpp"
#include "algebra/GammaMatrix.hpp"
#include "Operations.hpp"

namespace dirac {

namespace symbolic {

/**
 * Interpreter computes an expression
 * in inverse Polish (postfix) notation.
 */
template<typename Scalar>
class Interpreter {
public:
	/**
	 * Execute a single token.
	 * A value token is pushed to internal stack.
	 * An operation takes argument(s) from the stack,
	 * evaluates the result and pushes it to the stack.
	 */
	void exec(const Token<Scalar>& token);

	/**
	 * Execute an operation
	 */
	void exec(const Op& op);

	/**
	 * Execute a sequence of tokens
	 */
	template<std::input_iterator IToken, std::sentinel_for<IToken> IEnd>
	void exec(const IToken& begin, const IEnd& end) {
		for (IToken it = begin; it != end; ++it)
			exec(*it);
	}

	using OpStack = std::deque<OpList<Scalar>>;

	/**
	 * Returns a reference to the internal stack
	 */
	const OpStack& stack() const {
		return _stack;
	}
private:
	using BinaryOp =
			std::function<OpList<Scalar> (const OpList<Scalar>&,
											const OpList<Scalar>&)>;
	using UnaryOp =
			std::function<OpList<Scalar> (const OpList<Scalar>&)>;

	void performBinary(const BinaryOp& binOp);
	void performUnary(const UnaryOp& unaryOp);

	OpStack _stack;
};

//----------------------------------------------------------------------

template<typename Scalar>
void Interpreter<Scalar>::exec(const Token<Scalar>& token) {
	if (std::holds_alternative<Op>(token))
		exec(std::get<Op>(token));
	else if (std::holds_alternative<Scalar>(token)) {
		OpList<Scalar> value;
		value.push_back(std::get<Scalar>(token));
		_stack.push_front(value);
	} else if (std::holds_alternative<Literal>(token))
		_stack.push_front(OpList<Scalar>{ std::get<Literal>(token) });
}

//----------------------------------------------------------------------

template<typename Scalar>
void Interpreter<Scalar>::exec(const Op& op) {
	if (op == Op::Plus)
		performBinary([](const OpList<Scalar>& a,
				const OpList<Scalar>& b) -> OpList<Scalar> {
			return sum<Scalar>(a, b);
		});
	else if (op == Op::Minus)
		performBinary([](const OpList<Scalar>& a,
				const OpList<Scalar>& b) -> OpList<Scalar> {
			return diff<Scalar>(a, b);
		});
	else if (op == Op::UMinus)
		performUnary([](const OpList<Scalar>& a) {
			return neg<Scalar>(a);
		});
	else if (op == Op::Mul)
		performBinary([](const OpList<Scalar>& a,
				const OpList<Scalar>& b) -> OpList<Scalar> {
			return prod<Scalar>(a, b);
		});
	else if (op == Op::Div)
		performBinary([](const OpList<Scalar>& a,
				const OpList<Scalar>& b) -> OpList<Scalar> {
			return div<Scalar>(a, b);
		});
	else if (op == Op::Subs)
		performBinary([](const OpList<Scalar>& a,
				const OpList<Scalar>& b) -> OpList<Scalar> {
			return subscript<Scalar>(a, b);
		});
	else if (op == Op::Super)
		performBinary([](const OpList<Scalar>& a,
				const OpList<Scalar>& b) -> OpList<Scalar> {
			return superscript<Scalar>(a, b);
		});
	else if (op == Op::Splice)
		performBinary([](const OpList<Scalar>& a,
				const OpList<Scalar>& b) -> OpList<Scalar> {
			return join<Scalar>(a, b);
		});
	else
		throw std::runtime_error{
			"Unsupported operation: " + op.str() };
}

//----------------------------------------------------------------------

template<typename Scalar>
void Interpreter<Scalar>::performBinary(const BinaryOp &binOp) {
	if (_stack.size() < 2)
		throw std::runtime_error{
			"Not enough arguments for a binary operation" };

	OpList<Scalar>& second = _stack[0];
	OpList<Scalar>& first = _stack[1];
	OpList<Scalar> res = binOp(first, second);
	_stack.pop_front();
	_stack.pop_front();
	_stack.push_front(res);
}

//----------------------------------------------------------------------

template<typename Scalar>
void Interpreter<Scalar>::performUnary(const UnaryOp &unaryOp) {
	if (_stack.empty())
		throw std::runtime_error{
			"Not enough arguments for an unary operation" };

	OpList<Scalar> res = unaryOp(_stack.front());
	_stack.pop_front();
	_stack.push_front(res);
}

//----------------------------------------------------------------------

} /* namespace symbolic */

} /* namespace dirac */

#endif /* SRC_INTERPRETER_HPP_ */
