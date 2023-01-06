/*
 * ExprPrinter.hpp
 *
 *  Created on: Dec 22, 2022
 *      Author: skutnii
 */

#ifndef SRC_EXPRPRINTER_HPP_
#define SRC_EXPRPRINTER_HPP_

#include <string>
#include <unordered_map>
#include <sstream>

#include "algebra/Gamma.hpp"
#include "algebra/Rational.hpp"

#include "Operations.hpp"

namespace dirac {

namespace symbolic {

template<typename Scalar>
class ExprPrinter {
public:
	ExprPrinter(const std::string& dummyIndexName)
		: _dummyIndexName{ dummyIndexName } {}

	std::string latexify(const CanonicalExpr<Scalar>& expr);
	std::string latexify(const std::string& tensorHead,
							const TensorIndices& indices);

	std::string latexify(const algebra::LI::Tensor& t) {
		return latexify(t.id(), t.indices());
	}

	std::string latexify(const Scalar& s) {
		return std::to_string(s);
	}

	std::string latexify(const Complex<Scalar>& c);

	struct LatexTerm {
		std::string sign;
		std::string body;
	};

	using LatexTerms = std::vector<LatexTerm>;

	static std::string join(const LatexTerms& latexTerms) {
		std::stringstream latex;
		bool isFirst = true;
		for (const LatexTerm& term: latexTerms) {
			if (term.body.empty())
				continue;

			if (isFirst)
				isFirst = false;
			else
				latex << " " << term.sign << " ";

			latex << term.body;
		}

		return latex.str();
	}

	LatexTerms
	latexify(const algebra::LI::TensorPolynomial<Scalar>& poly);

	std::string mapIndexId(const algebra::IndexId& aId);
private:
	std::string _dummyIndexName;
	std::unordered_map<algebra::IndexTag, std::string> _indexTagMap;
};

extern const std::string leftBrace;
extern const std::string rightBrace;
extern const std::string hat;
extern const std::string subs;
extern const std::string leftSquareBracket;
extern const std::string rightSquareBracket;
extern const std::string leftBracket;
extern const std::string rightBracket;

template<typename Scalar>
std::string
ExprPrinter<Scalar>::latexify(const std::string& head,
								const TensorIndices& indices) {
	using namespace algebra;
	//first: upper or lower flag; second: body
	using Fragment = std::pair<bool, std::string>;
	std::vector<Fragment> frags;

	/*
	 Concatenate adjacent subscript
		or superscript indices into fragments
	*/
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

		value += (frag.first ? hat : subs)
				+ leftBrace + frag.second + rightBrace;
	}


	return value;
}

template<>
std::string
ExprPrinter<algebra::Rational>::latexify(const algebra::Rational& r);

template<typename Scalar>
std::string ExprPrinter<Scalar>::latexify(const Complex<Scalar>& c) {
	bool hasReal = (c.real() != 0);
	bool hasImag = (c.imag() != 0);

	std::string value;
	if (hasReal)
		value += latexify(c.real());

	if (hasReal && (c.imag() > 0))
		value += " + ";

	if (hasImag)
		value += latexify(c.imag()) + "I";

	return value;
}

template<>
std::string
ExprPrinter<algebra::Rational>::latexify(
		const Complex<algebra::Rational>& c);

template<typename Scalar>
std::string sign(const Complex<Scalar>& c) {
	Scalar zero = static_cast<Scalar>(0);
	if ((c.real() != zero) && (c.imag() != zero))
		return "+";
	else if (c.imag() == zero)
		return (c.real() > zero) ? "+" : "";
	else
		return (c.imag() > zero) ? "+" : "";
}

template<typename Scalar>
typename ExprPrinter<Scalar>::LatexTerms
ExprPrinter<Scalar>::latexify(
		const algebra::LI::TensorPolynomial<Scalar>& poly) {
	using namespace algebra;

	LatexTerms latex;
	latex.reserve(poly.terms.size());

	for (const typename LI::TensorPolynomial<Scalar>::Term&
											term : poly.terms) {
		if (term.coeff == zero<Scalar>())
			continue;

		latex.emplace_back();
		LatexTerm& latexTerm = latex.back();

		for (const LI::Tensor& factor : term.factors)
			latexTerm.body += latexify(factor);

		latexTerm.sign = sign(term.coeff);
		if (term.coeff == one<Scalar>())
			continue;

		if (term.coeff == -one<Scalar>()) {
			latexTerm.body = "-" + latexTerm.body;
			continue;
		}

		std::string coeff = latexify(term.coeff);
		if ((term.coeff.real() != 0) && (term.coeff.imag() != 0))
			coeff = leftBracket + coeff + rightBracket;

		latexTerm.body = coeff + latexTerm.body;
	}

	return latex;
}

template<typename Scalar>
std::string ExprPrinter<Scalar>::mapIndexId(
									const algebra::IndexId& aId) {
	using namespace algebra;

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

template<typename Scalar>
std::string
ExprPrinter<Scalar>::latexify(const CanonicalExpr<Scalar>& expr) {
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
			terms[i].body = leftSquareBracket
					+ join(latexCoeffs[i]) + rightSquareBracket;
			terms[i].sign = "+";
		} else {
			terms[i] = latexCoeffs[i][0];
		}
	}

	if (!latexCoeffs[1].empty())
		terms[1].body += latexify(std::string{ "\\gamma" },
										{ expr.vectorIndex });

	if (!latexCoeffs[2].empty())
		terms[2].body += latexify(std::string{ "\\sigma" },
									{ expr.tensorIndices.first,
										expr.tensorIndices.second });

	if (!latexCoeffs[3].empty()) {
		terms[3].body += "\\gamma^5";
		terms[3].body += latexify(std::string{ "\\gamma" },
									{ expr.pseudoVectorIndex });
	}

	if (!latexCoeffs[4].empty())
		terms[4].body += "\\gamma^5";

	return join(terms);
}

} /* namespace symbolic */

} /* namespace dirac */

#endif /* SRC_EXPRPRINTER_HPP_ */
