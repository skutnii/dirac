/*
 * Processor.hpp
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

#include "algebra/GammaMatrix.hpp"
#include "Symbolic.hpp"
#include "Operations.hpp"

namespace dirac {

class Interpreter {
public:

	void exec(const Token& token);
	void exec(const Op& op);

	template<std::input_iterator IToken, std::sentinel_for<IToken> IEnd>
	void exec(const IToken& begin, const IEnd& end) {
		for (IToken it = begin; it != end; ++it)
			exec(*it);
	}

	using OpStack = std::deque<OpList>;

	const OpStack& stack() const {
		return _stack;
	}
private:
	using BinaryOp = std::function<OpList (const OpList&, const OpList&)>;
	using UnaryOp = std::function<OpList (const OpList&)>;

	void performBinary(const BinaryOp& binOp);
	void performUnary(const UnaryOp& unaryOp);

	OpStack _stack;
};

} /* namespace dirac */

#endif /* SRC_INTERPRETER_HPP_ */
