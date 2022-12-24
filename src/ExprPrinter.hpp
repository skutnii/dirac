/*
 * ExprPrinter.hpp
 *
 *  Created on: Dec 22, 2022
 *      Author: skutnii
 */

#ifndef SRC_EXPRPRINTER_HPP_
#define SRC_EXPRPRINTER_HPP_

#include "Symbolic.hpp"
#include <string>
#include <unordered_map>
#include <sstream>

namespace dirac {

namespace symbolic {

class ExprPrinter {
public:
	ExprPrinter(const std::string& dummyIndexName)
		: _dummyIndexName{ dummyIndexName } {}

	std::string latexify(const CanonicalExpr& expr);
	std::string latexify(const std::string& tensorHead, const TensorIndices& indices);

	std::string latexify(const algebra::LI::Tensor& t) {
		return latexify(t.id(), t.indices());
	}

	struct LatexTerm {
		std::string sign;
		std::string body;
	};

	using LatexTerms = std::vector<LatexTerm>;

	static std::string join(const LatexTerms& latexTerms) {
		std::stringstream latex;
		bool isFirst = true;
		for (const LatexTerm& term: latexTerms) {
			if (isFirst)
				isFirst = false;
			else
				latex << " " << term.sign << " ";

			latex << term.body;
		}

		return latex.str();
	}

	LatexTerms latexify(const algebra::LI::TensorPolynomial& poly);

	std::string mapIndexId(const algebra::IndexId& aId);
private:
	std::string _dummyIndexName;
	std::unordered_map<algebra::IndexTag, std::string> _indexTagMap;
};

} /* namespace symbolic */

} /* namespace dirac */

#endif /* SRC_EXPRPRINTER_HPP_ */
