/*
 * compile.h
 *
 *  Created on: Dec 12, 2022
 *      Author: skutnii
 */

#ifndef SRC_COMPILER_HPP_
#define SRC_COMPILER_HPP_

#include <deque>
#include <optional>
#include <functional>
#include <list>
#include <memory>

#include "InputSequence.hpp"
#include "Token.hpp"

namespace dirac {

template<typename Number>
using Executable = std::list<Token<Number>>;

/*
 * Compiler transforms a sequence of tokens in natural order
 * into reverse Polish (postfix) order suitable for computation.
 */
template<typename Number>
class Compiler {
public:
	/**
	 * Returns true at the beginning of an expression or immediately after a bracket;
	 * false otherwise
	 */
	bool isNewSubexpr() {
		return ((_state == Empty) || (_state == LBrace));
	}

	void pushToken(const Token<Number>& token) {
		if (isOp(token))
			pushOp(std::get<Op>(token));
		else
			pushValue(token);
	}

	void pushOp(const Op& op);

	/*
	 * Move all operations from internal stack to the body
	 * as if at the end of an expression.
	 */
	void popAll();

	template<InputSequence<Number> Input>
	void compile(Input& input) {
		while (std::optional<Token<Number>> maybeToken = input.nextToken())
			pushToken(maybeToken.value());

		popAll();
	}

	//Operator precedence
	enum Precedence {
		Nop = -1,
		Bracket = 0,
		Additive = 1,
		Multiplicative = 2,
		Unary = 3,
		Subscript = 4
	};

	static Precedence precedence(const Op& op);

	/*
	 * Executable code in reverse Polish (postfix) notation
	 */
	const Executable<Number>& opCode() const { return _body; }
private:
	void internalError() {
		throw std::runtime_error{
			"Internal error: inconsistent compiler state." };
	}

	void pushValue(const Token<Number>& valueToken);

	//Unconditionally push an operation
	void doPush(const Op& op);

	/**
	 * Flushes operations from the stack until its argument
	 * evaluates to true or throws an exception.
	 * Exceptions are not handled.
	 */
	void popUntil(const std::function<bool ()>& cond);

	enum State {
		Empty = 0,
		LBrace,
		RBrace,
		Operator,
		Value
	};

	State _state = Empty;
	Executable<Number> _body;
	std::deque<Op> _opStack;
	std::optional<Token<Number>> _lastToken;
};

template<typename Number>
void Compiler<Number>::pushValue(const Token<Number>& valueToken) {
	/*
	 * Promote consecutive values to list concatenation
	 * \a\b -> \a|\b.
	 * Also }\a = }|\a.
	 */
	if ((_state == Value) || (_state == RBrace))
		pushOp(Op::Splice);

	_body.push_back(valueToken);
	_lastToken = valueToken;
	_state = Value;
}

template<typename Number>
void Compiler<Number>::pushOp(const Op &op) {
	if (op == Op::Nop)
		throw std::runtime_error{ "Invalid operation: " + op.str() };

	/*
	 * Promote minus to unary at the beginning of a (sub)expression
	 */
	if ((op == Op::Minus) && isNewSubexpr()) {
		doPush(Op::UMinus);
		return;
	}

	/*
	 * Check for two consecutive operators.
	 * Note that opening bracket may stand anywhere
	 * and closing bracket may be followed by anything.
	 */
	if (op != Op::LBrace) {
		if (_state == Empty)
			throw std::runtime_error{
				"Syntax error: expressions cannot start with " + op.str()};

		bool isAfterBracket = (_state == LBrace);
		if (isAfterBracket || (_state == Operator)) {
			bool isBracket = (op == Op::RBrace);

			if (isBracket && isAfterBracket)
				throw std::runtime_error{ "Syntax error: empty bracket" };

			if (!isBracket && isAfterBracket)
				throw std::runtime_error{
					"Syntax error: " + op.str() + " cannot follow an opening bracket" };

			if (isBracket && !isAfterBracket)
				throw std::runtime_error{
					"Syntax error: closing bracket after " + op.str() };

			if (!isBracket && !isAfterBracket)
				throw std::runtime_error{
					"Syntax error: consecutive operators "
					+ std::get<Op>(_lastToken.value()).str()
					+ " and " + op.str() };
		}
	}

	/*
	 * Process opening bracket
	 */
	if (op == Op::LBrace) {
		if ((_state == Value) || (_state == RBrace))
			pushOp(Op::Splice);

		doPush(op);
		return;
	}

	/*
	 * Process closing bracket
	 */
	if (op == Op::RBrace) {
		popUntil(
			[this]() {
				if (_opStack.empty())
					throw std::runtime_error{
						"Syntax error: unmatched closing bracket" };

				return (_opStack.front() == Op::LBrace);
			});

		_opStack.pop_front(); //Remove the opening bracket
		_state = RBrace;
		_lastToken = op;
		return;
	}

	Precedence opPriority = precedence(op);
	popUntil(
		[this, opPriority]() {
			if (_opStack.empty())
				return true;

			Precedence prevPriority = precedence(_opStack.front());
			return (opPriority > prevPriority);
		});

	doPush(op);
}

template<typename Number>
typename Compiler<Number>::Precedence
Compiler<Number>::precedence(const Op& op) {
	if ((op == Op::LBrace) || (op == Op::RBrace))
		return Bracket;

	if ((op == Op::Minus) || (op == Op::Plus))
		return Additive;

	if ((op == Op::Mul) || (op == Op::Div) || (op == Op::Splice))
		return Multiplicative;

	if (op == Op::UMinus)
		return Unary;

	if ((op == Op::Subs) || (op == Op::Super))
		return Subscript;

	throw std::runtime_error{
		"Could not determine precedence of operation " + op.str() };

	return Nop;
}

template<typename Number>
void Compiler<Number>::doPush(const Op& op) {
	_opStack.push_front(op);
	_lastToken = op;

	if (op == Op::LBrace)
		_state = LBrace;
	else if (op == Op::RBrace)
		_state = RBrace;
	else
		_state = Operator;
}

template<typename Number>
void Compiler<Number>::popUntil(const std::function<bool()> &cond) {
	if (cond())
		return;

	Executable<Number> fragment;
	do {
		Op topOp = _opStack.front();
		_opStack.pop_front();
		_body.push_back(topOp);
	} while(!cond());
}

template<typename Number>
void Compiler<Number>::popAll() {
	popUntil(
		[this]() {
			if (_opStack.empty())
				return true;

			Op& topOp = _opStack.front();
			if (topOp == Op::LBrace)
				throw std::runtime_error{
					"Syntax error: unmatched opening bracket" };

			if ((topOp == Op::UMinus) && _body.empty())
				throw std::runtime_error{
					"Syntax error: unary minus requires an argument" };
			else if (_body.size() < 2)
				throw std::runtime_error{
					"Syntax error: " + topOp.str() + " requires two arguments." };

			return false;
		});
}

}

#endif /* SRC_COMPILER_HPP_ */
