/*
 * ExprPrinter.hpp
 *
 * LaTeX printer for canonical gamma-matrix exprressions
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

/**
 * LaTeX printer for gamma-matrix expressions
 */
template<typename Scalar>
class ExprPrinter {
public:
	/**
	 * Constructor.
	 *
	 * @param dummyIndexName - dummy index name template,
	 * 		  used to print indices with non-string identifiers.
	 * 		  See mapIndexId
	 *
	 * @param lineSize - number of terms per line.
	 * 		  If this parameter is nonzero,
	 * 		  the printer inserts line breaks after each lineSize terms.
	 * 		  The line breaks are in LaTeX format, as if inside
	 * 		  'split' LaTeX environment.
 	 */
	ExprPrinter(const std::string& dummyIndexName, size_t lineSize)
		: _dummyIndexName{ dummyIndexName }, _lineSize{ lineSize } {}

	/**
	 * Main pretty-printing method.
	 * Converts a canonical expression to LaTeX.
	 */
	std::string latexify(const CanonicalExpr<Scalar>& expr);

	/**
	 * LaTeX representation of a tensorial expression
	 * (that is, anything with indices).
	 */
	std::string latexify(const std::string& tensorHead,
							const TensorIndices& indices);

	/**
	 * LaTeX representation of a Lorentz-invariant (pseudo)-tensor
	 */
	std::string latexify(const algebra::LI::Tensor& t) {
		return latexify(t.id(), t.indices());
	}

	/**
	 * LaTeX representation of a number.
	 * Default implementation calls std::to_string
	 */
	std::string latexify(const Scalar& s) {
		return std::to_string(s);
	}

	/**
	 * LaTeX representation of a complex number
	 */
	std::string latexify(const Complex<Scalar>& c);

	/**
	 * Helper structure for LaTeX representation of a single term
	 * in a polynomial
	 */
	struct LatexTerm {
		/**
		 * Sign that may be prepended to a term.
		 * "+" or empty
		 */
		std::string sign;

		/**
		 * Term body
		 */
		std::string body;

		LatexTerm(const std::string& s, const std::string& b) :
											sign{ s }, body{ b } {}

		LatexTerm() = default;
		LatexTerm(const LatexTerm& other) = default;
		LatexTerm(LatexTerm&& other) = default;

		LatexTerm& operator=(const LatexTerm& other) = default;
	};

	using LatexTerms = std::vector<LatexTerm>;

	/**
	 * Joins terms into a string
	 */
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

	/**
	 * LaTeX representation of a Lorentz-invariant
	 * (pseudo)-tensor polynomial.
	 */
	LatexTerms
	latexify(const algebra::LI::TensorPolynomial<Scalar>& poly);

	/**
	 * If the index identifier is a string, returns it.
	 * Otherwise maps it to a string using the dummyIndexName
	 * provided to the printer's constructor.
	 *
	 * Internally, the printer instance maintains a table
	 * of encountered non-string index identifiers.
	 * If dummyIndexName is e.g. \omega, then the first
	 * encountered index identifier will be mapped to
	 * \omega_1, the second one to \omega_2 and so on.
	 */
	std::string mapIndexId(const algebra::IndexId& aId);

	using TermGroups = std::vector<LatexTerms>;

	std::string latexify(const TermGroups& termGroups);
private:
	std::string _dummyIndexName;
	size_t _lineSize;
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

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

/**
 * Specialization for LaTeX representation of rationals
 */
template<>
std::string
ExprPrinter<algebra::Rational>::latexify(const algebra::Rational& r);

//----------------------------------------------------------------------

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
		if (c.imag() == static_cast<Scalar>(1))
			value += "I";
		else if (c.imag() == static_cast<Scalar>(-1))
			value += "-I";
		else
			value += latexify(c.imag()) + "I";

	return value;
}

//----------------------------------------------------------------------

/**
 * Specialization for LaTeX representation of a complex number
 * with rational constituents.
 */
template<>
std::string
ExprPrinter<algebra::Rational>::latexify(
		const Complex<algebra::Rational>& c);

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

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

//----------------------------------------------------------------------

template<typename Scalar>
std::string
ExprPrinter<Scalar>::latexify(const CanonicalExpr<Scalar>& expr) {
	if (expr.isZero())
		return "0";

	if (expr.isScalar(static_cast<Scalar>(1)))
		return "1";

	if (expr.isScalar(static_cast<Scalar>(-1)))
		return "-1";

	TermGroups latexCoeffs {
			latexify(expr.coeffs(0)),
			latexify(expr.coeffs(1)),
			latexify(expr.coeffs(2)),
			latexify(expr.coeffs(3)),
			latexify(expr.coeffs(4))
	};

	//Add brackets if necessary
	for (size_t i = 1; i < 5; ++i) {
		if (latexCoeffs[i].empty())
			continue;

		if (latexCoeffs[i].size() > 1) {
			latexCoeffs[i][0].body =
					leftSquareBracket + latexCoeffs[i][0].body;
			latexCoeffs[i][0].sign = "+";
			latexCoeffs[i].back().body += rightSquareBracket;
		}
	}

	if (!latexCoeffs[1].empty())
		latexCoeffs[1].back().body += latexify(std::string{ "\\gamma" },
										{ expr.vectorIndex });

	if (!latexCoeffs[2].empty())
		latexCoeffs[2].back().body += latexify(std::string{ "\\sigma" },
									{ expr.tensorIndices.first,
										expr.tensorIndices.second });

	if (!latexCoeffs[3].empty())
		latexCoeffs[3].back().body += std::string{"\\gamma^5"}
								+ latexify(std::string{ "\\gamma" },
											{ expr.pseudoVectorIndex });


	if (!latexCoeffs[4].empty())
		latexCoeffs[4].back().body += "\\gamma^5";

	return latexify(latexCoeffs);
}

//----------------------------------------------------------------------

template<typename Scalar>
std::string
ExprPrinter<Scalar>::latexify(
		const ExprPrinter<Scalar>::TermGroups& latexCoeffs) {
	LatexTerms allTerms;
	size_t termCount = 0;
	for (size_t i = 0; i < latexCoeffs.size(); ++i)
		for (size_t j = 0; j < latexCoeffs[i].size(); ++j) {
			LatexTerm term = latexCoeffs[i][j];

			//Add line break
			if (_lineSize > 0) {
				if (0 == termCount)
					term.body = std::string{ "&" } + term.body;
				else if ((termCount % _lineSize) == 0) {
					std::string lineBreak =
							((latexCoeffs[i].size() > 0)
									&& (j > 0) && (i > 0)) ?
											"\\right.\\\\\n&\\left."
											: "\\\\\n&";
					if (term.sign.empty())
						term.body = "-" + lineBreak + term.body;
					else
						term.body = lineBreak + term.sign + term.body;
				}
			}

			allTerms.push_back(term);

			++termCount;
		}


	if (allTerms.empty())
		return "0";

	return join(allTerms);
}

//----------------------------------------------------------------------

} /* namespace symbolic */

} /* namespace dirac */

#endif /* SRC_EXPRPRINTER_HPP_ */
