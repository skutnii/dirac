/*
 * Token.cpp
 *
 *  Created on: Dec 12, 2022
 *      Author: skutnii
 */

#include "Token.h"

namespace dirac {

const Op Op::Nop{}; //Not an operation
const Op Op::Plus{'+'}; // +
const Op Op::Minus{'-'}; // binary -
const Op Op::UMinus{'~'}; // unary -
const Op Op::Mul{'*'}; // *
const Op Op::Div{'/'}; // /
const Op Op::LBrace{'{'}; // {
const Op Op::RBrace{'}'}; // }
const Op Op::Subs{'_'}; // subscript _
const Op Op::Super{'^'}; // superscript ^
const Op Op::Splice {'&'}; // list concatenation

std::ostream& operator<<(std::ostream &os, const Token &token) {
	if (const Op* op = std::get_if<Op>(&token))
		return (os << *op);

	if (const Number* num = std::get_if<Number>(&token))
		return (os << *num);

	return (os << *std::get_if<std::string>(&token));
}

}
