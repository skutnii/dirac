/*
 * Tokeniser.h
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#ifndef INPUT_H_
#define INPUT_H_

#include <string>
#include <unordered_set>

#include "InputSequence.hpp"
#include "Token.hpp"

namespace dirac {

/**
 * Splits input into meaningful fragments
 */
class StringInput : public InputSequence {
public:
	StringInput(const std::string& src)
		: _content{src.begin(), src.end()} {};

	std::optional<Token> nextToken() override;

	bool empty() const { return _content.empty(); }

	virtual ~StringInput() = default;

	using CharSet = std::unordered_set<char>;

	static const CharSet ops;

private:
	void skipSpace();
	Number nextNumber();
	std::string nextSymbol();

	void skipTo(std::string_view::iterator& it);
	void skipTo(size_t pos);

	std::string_view _content;
};

} /* namespace dirac */

#endif /* INPUT_H_ */
