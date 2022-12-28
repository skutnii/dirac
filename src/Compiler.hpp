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

using Executable = std::list<Token>;

/*
 * Compiler transforms a sequence of tokens in natural order
 * into reverse Polish (postfix) order suitable for computation.
 */
class Compiler {
public:
	/**
	 * Returns true at the beginning of an expression or immediately after a bracket;
	 * false otherwise
	 */
	bool isNewSubexpr() {
		return ((_state == Empty) || (_state == LBrace));
	}

	void pushToken(const Token& token) {
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

	void compile(InputSequence& input);

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
	const Executable& opCode() const { return _body; }
private:
	void internalError() {
		throw std::runtime_error{
			"Internal error: inconsistent compiler state." };
	}

	void pushValue(const Token& valueToken);

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
	Executable _body;
	std::deque<Op> _opStack;
	std::optional<Token> _lastToken;
};

}

#endif /* SRC_COMPILER_HPP_ */
