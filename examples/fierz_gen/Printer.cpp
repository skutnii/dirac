/*
 * Printer.cpp
 *
 *  Created on: Jan 27, 2023
 *      Author: skutnii
 */

#include "Printer.hpp"

namespace fierz {

//----------------------------------------------------------------------

std::string Printer::latexify(const Bilinear &expr,
		const std::pair<std::string, std::string>& spinorIndices) {
	std::string res = std::string{ "\\bar{\\psi}_{" }
							+ spinorIndices.first + "}";

	switch (expr.id()) {
	case 1:
		res += Base::latexify(std::string{ "\\gamma" }, expr.indices());
		break;
	case 2:
		res += Base::latexify(std::string{ "\\sigma" }, expr.indices());
		break;
	case 3:
		res += "\\gamma^5";
		res += Base::latexify(std::string{ "\\gamma" }, expr.indices());
		break;
	case 4:
		res += "\\gamma^5";
		break;
	default:
		break;
	}

	res += std::string{ "\\psi_{" } + spinorIndices.second + "}";
	return res;
}

//----------------------------------------------------------------------

Printer::LatexTerms Printer::latexify(const Expression::Term &term,
		const std::vector<
				std::pair<std::string, std::string> > &spinorIndices) {
	if (term.coeff.isZero())
		return {};

	LatexTerms coeffTerms = Base::latexify(term.coeff);

	if (coeffTerms.size() > 1) {
		coeffTerms[0].body =
				std::string{ "\\left[" } + coeffTerms[0].body;
		coeffTerms[0].sign = "+";
		coeffTerms.back().body += "\\right]";
	}

	coeffTerms.back().body += latexify(term.factors, spinorIndices);

	return coeffTerms;
}

//----------------------------------------------------------------------

std::string Printer::latexify(const Expression &expr,
		const std::vector<
				std::pair<std::string, std::string> >& spinorIndices) {
	size_t termCount = expr.terms.size();

	TermGroups terms;
	for (const Expression::Term& term : expr.terms)
		terms.push_back(latexify(term, spinorIndices));

	return Base::latexify(terms);
}

//----------------------------------------------------------------------

std::string Printer::latexify(const std::vector<Bilinear>& expr,
		const std::vector<std::pair<std::string, std::string> >&
		spinorIndices) {
	size_t factorCount = expr.size();
	if (spinorIndices.size() < factorCount)
		throw std::runtime_error{ "Not enough spinor indices" };

	std::string res;

	for (size_t iFac = 0; iFac < factorCount; ++iFac)
		res += latexify(expr[iFac], spinorIndices[iFac]);

	return res;
}

//----------------------------------------------------------------------

std::string Printer::latexify(const Identity& identity) {
	std::stringstream out;

	if (identity.left.terms.empty())
		throw std::runtime_error{ "Invalid identity" };

	TermGroups leftParts;
	leftParts.reserve(identity.left.terms.size());

	for (const Expression::Term& term : identity.left.terms)
		leftParts.push_back(latexify(term, identity.leftSpinorIndices));

	if (leftParts[0].empty()) {
		leftParts.emplace_back();
		LatexTerm zero;
		zero.body = "0";
		zero.sign = "=";
		leftParts[0].push_back(zero);
	}

	TermGroups rightParts;
	rightParts.reserve(identity.right.terms.size());
	for (const Expression::Term& term : identity.right.terms)
		rightParts.push_back(latexify(term,
				identity.rightSpinorIndices));

	while(!rightParts.empty() && rightParts[0].empty())
		rightParts.erase(rightParts.begin());

	if (rightParts.empty()) {
		rightParts.emplace_back();
		LatexTerm zero;
		zero.body = "0";
		zero.sign = "";
		rightParts[0].push_back(zero);
	}

	rightParts[0][0].sign = "=";

	TermGroups parts;
	parts.insert(parts.end(), leftParts.begin(), leftParts.end());
	parts.insert(parts.end(), rightParts.begin(), rightParts.end());

	out << "\\begin{equation}" << std::endl
			<< "\\begin{split}" << std::endl
			<< Base::latexify(parts) << std::endl
			<< "\\end{split}" << std::endl
			<< "\\end{equation}" << std::endl;

	return out.str();
}

} /* namespace fierz */
