/*
 * Printer.hpp
 *
 *  Created on: Jan 27, 2023
 *      Author: skutnii
 */

#ifndef EXAMPLES_FIERZ_GEN_PRINTER_HPP_
#define EXAMPLES_FIERZ_GEN_PRINTER_HPP_

#include "defs.hpp"
#include "ExprPrinter.hpp"
#include "Expression.hpp"
#include "Identity.hpp"

namespace fierz {

class Printer: public dirac::symbolic::ExprPrinter<Rational> {
public:
	using Base = dirac::symbolic::ExprPrinter<Rational>;
	using LatexTerms = Base::LatexTerms;
	using TermGroups = Base::TermGroups;

	Printer(const std::string& dummyIndexName,
			size_t lineSize) : Base{ dummyIndexName, lineSize } {}

	/**
	 * Convert a Fierz bilinear to LaTeX form
	 */
	std::string latexify(const Bilinear& expr,
			const std::pair<std::string, std::string>& spinorIndices);

	/**
	 * Convert a Fierz multilinear to LaTeX form
	 */
	std::string latexify(const std::vector<Bilinear>& expr,
			const std::vector<std::pair<std::string, std::string> >&
			spinorIndices);


	/**
	 * Convert an expression term to LaTeX form
	 */
	LatexTerms latexify(const Expression::Term& term,
			const std::vector<
					std::pair<
						std::string, std::string>>& spinorIndices);

	/**
	 * Convert expression to LaTeX form
	 */
	std::string latexify(const Expression& expr,
			const std::vector<
					std::pair<
					std::string, std::string>>& spinorIndices);

	std::string latexify(const Identity& identity);
};

} /* namespace fierz */

#endif /* EXAMPLES_FIERZ_GEN_PRINTER_HPP_ */
