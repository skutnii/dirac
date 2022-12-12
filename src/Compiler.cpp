/*
 * compile.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: skutnii
 */

#include "Compiler.h"

namespace dirac {

void Compiler::pushValue(const Token &valueToken) {
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

void Compiler::pushOp(const Op &op) {
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

Compiler::Precedence
Compiler::precedence(const Op& op) {
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

void Compiler::doPush(const Op& op) {
	_opStack.push_front(op);
	_lastToken = op;

	if (op == Op::LBrace)
		_state = LBrace;
	else if (op == Op::RBrace)
		_state = RBrace;
	else
		_state = Operator;
}

void Compiler::popUntil(const std::function<bool()> &cond) {
	if (cond())
		return;

	Executable fragment;
	do {
		Op topOp = _opStack.front();
		_opStack.pop_front();
		_body.push_back(topOp);
	} while(!cond());
}

void Compiler::popAll() {
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

void Compiler::compile(InputSequence& input) {
	while (std::optional<Token> maybeToken = input.nextToken())
		pushToken(maybeToken.value());

	popAll();
}

}
