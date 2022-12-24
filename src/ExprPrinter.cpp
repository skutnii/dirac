/*
 * ExprPrinter.cpp
 *
 *  Created on: Dec 22, 2022
 *      Author: skutnii
 */

#include "ExprPrinter.hpp"
#include <string>

namespace dirac {

namespace symbolic {

using namespace algebra;

static const std::string leftBrace{ "{" };
static const std::string rightBrace{ "}" };
static const std::string hat{ "^" };
static const std::string subs{ "_" };
static const std::string leftSquareBracket{ "\\left\\[" };
static const std::string rightSquareBracket{ "\\right\\]" };
static const std::string leftBracket{ "\\(" };
static const std::string rightBracket{ "\\)" };

std::string
ExprPrinter::latexify(const std::string& head, const TensorIndices& indices) {
	std::string value = head;

	for (const TensorIndex& idx : indices)
		value = leftBrace + value + rightBrace +
				(idx.isUpper ? hat : subs) +
						leftBrace + mapIndexId(idx.id) + rightBrace;

	return value;
}

static std::string to_string(const Complex& c) {
	std::string value;

	if (c.real() != 0)
		value += std::to_string(c.real());

	if (c.imag() > 0)
		value += "+";

	if (c.imag() != 0) {
		value += std::to_string(c.imag());
		value += "i";
	}

	return value;
}

static std::string sign(const Complex& c) {
	if ((c.real() != 0) && (c.imag() != 0))
		return "+";
	else if (c.imag() == 0)
		return (c.real() > 0) ? "+" : "";
	else
		return (c.imag() > 0) ? "+" : "";
}

ExprPrinter::LatexTerms
ExprPrinter::latexify(const LI::TensorPolynomial& poly) {
	LatexTerms latex;
	latex.reserve(poly.terms.size());

	for (const LI::TensorPolynomial::Term& term : poly.terms) {
		if (term.coeff == zero())
			continue;

		latex.emplace_back();
		LatexTerm& latexTerm = latex.back();
		latexTerm.sign = sign(term.coeff);
		std::string coeff = to_string(term.coeff);
		if ((term.coeff.real() != 0) && (term.coeff.imag() != 0))
			coeff = leftBracket + coeff + rightBracket;

		latexTerm.body += coeff;

		for (const LI::Tensor& factor : term.factors)
			latexTerm.body += latexify(factor);
	}

	return latex;
}

std::string ExprPrinter::mapIndexId(const IndexId& aId) {
	if (std::holds_alternative<std::string>(aId))
		return std::get<std::string>(aId);

	const IndexTag& tag = std::get<IndexTag>(aId);
	auto iTag = _indexTagMap.find(tag);
	if (iTag != _indexTagMap.end())
		return iTag->second;

	std::string tagStr = _dummyIndexName
			+ "_{" + std::to_string(_indexTagMap.size() + 1) + "}";
	_indexTagMap[tag] = tagStr;
	return tagStr;
}

std::string ExprPrinter::latexify(const CanonicalExpr& expr) {
	LatexTerms latexCoeffs[] = {
			latexify(expr.scalar.coeff),
			latexify(expr.vector.coeff),
			latexify(expr.tensor.coeff),
			latexify(expr.pseudoVector.coeff),
			latexify(expr.pseudoScalar.coeff)
	};

	LatexTerms terms;
	terms.resize(5);
	for (size_t i = 0; i < 5; ++i) {
		if (latexCoeffs[i].empty())
			continue;

		if (latexCoeffs[i].size() > 1) {
			terms[i].body = leftSquareBracket + join(latexCoeffs[i]) + rightSquareBracket;
			terms[i].sign = "+";
		} else {
			terms[i] = latexCoeffs[i][0];
		}
	}

	if (!latexCoeffs[1].empty())
		terms[1].body += latexify(std::string{ "\\gamma" }, expr.vector.indices);

	if (!latexCoeffs[2].empty())
		terms[2].body += latexify(std::string{ "\\sigma" }, expr.tensor.indices);

	if (!latexCoeffs[3].empty()) {
		terms[3].body += "\\gamma^5";
		terms[3].body += latexify(std::string{ "\\gamma" }, expr.pseudoVector.indices);
	}

	if (!latexCoeffs[4].empty())
		terms[4].body += "\\gamma^5";

	return join(terms);
}

} /* namespace symbolic */

} /* namespace dirac */
