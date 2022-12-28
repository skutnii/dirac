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
static const std::string leftSquareBracket{ "\\left[" };
static const std::string rightSquareBracket{ "\\right]" };
static const std::string leftBracket{ "\\left(" };
static const std::string rightBracket{ "\\right)" };

std::string
ExprPrinter::latexify(const std::string& head, const TensorIndices& indices) {
	using Fragment = std::pair<bool, std::string>; //first: upper or lower flag; second: body
	std::vector<Fragment> frags;

	//Concatenate adjacent subscript or superscript indices into fragments
	for (const TensorIndex& idx : indices) {
		if (frags.empty()
				|| (frags.back().first && !idx.isUpper)
				|| (!frags.back().first && idx.isUpper))
			frags.emplace_back(idx.isUpper, "");

		frags.back().second += mapIndexId(idx.id);
	}

	std::string value = head;

	//Now attach subscript and superscript fragments to the head
	bool firstFrag = true;
	for (auto& frag : frags) {
		if (firstFrag)
			firstFrag = false;
		else
			value = leftBrace + value + rightBrace;

		value += (frag.first ? hat : subs) + leftBrace + frag.second + rightBrace;
	}


	return value;
}

static std::string to_string(const Complex& c) {
	bool hasReal = (c.real() != 0);
	bool hasImag = (c.imag() != 0);

	std::string value;
	if (hasReal)
		value += std::to_string(c.real());

	if (hasReal && (c.imag() > 0))
		value += " + ";

	if (hasImag)
		value += std::to_string(c.imag()) + "I";

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

		for (const LI::Tensor& factor : term.factors)
			latexTerm.body += latexify(factor);

		latexTerm.sign = sign(term.coeff);
		if (term.coeff == one())
			continue;

		if (term.coeff == -one()) {
			latexTerm.body = "-" + latexTerm.body;
			continue;
		}

		std::string coeff = to_string(term.coeff);
		if ((term.coeff.real() != 0) && (term.coeff.imag() != 0))
			coeff = leftBracket + coeff + rightBracket;

		latexTerm.body = coeff + latexTerm.body;
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
			latexify(expr.coeffs(0)),
			latexify(expr.coeffs(1)),
			latexify(expr.coeffs(2)),
			latexify(expr.coeffs(3)),
			latexify(expr.coeffs(4))
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
		terms[1].body += latexify(std::string{ "\\gamma" }, { expr.vectorIndex });

	if (!latexCoeffs[2].empty())
		terms[2].body += latexify(std::string{ "\\sigma" },
				{ expr.tensorIndices.first, expr.tensorIndices.second });

	if (!latexCoeffs[3].empty()) {
		terms[3].body += "\\gamma^5";
		terms[3].body += latexify(std::string{ "\\gamma" }, { expr.pseudoVectorIndex });
	}

	if (!latexCoeffs[4].empty())
		terms[4].body += "\\gamma^5";

	return join(terms);
}

} /* namespace symbolic */

} /* namespace dirac */
