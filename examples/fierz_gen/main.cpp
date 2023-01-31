/*
 * main.cpp
 *
 *  Created on: Jan 23, 2023
 *      Author: skutnii
 */

#include <iostream>
#include <fstream>

#include "Expression.hpp"
#include "algorithms.hpp"
#include "Identity.hpp"
#include "Printer.hpp"

int main(int argc, char** argv) {
	using namespace fierz;
	using namespace dirac::algebra;

	std::cout << "Sixth-order Fierz identities generator" << std::endl;

	std::ofstream out{ "fierz.tex" };
	out << "\\documentclass[aps,prd,a4paper]{revtex4-2}" << std::endl
		<< "\\usepackage[T1]{fontenc}" << std::endl
		<< "\\usepackage{underscore}" << std::endl
		<< "\\usepackage{amsmath}" << std::endl
		<< "\\usepackage{amssymb}" << std::endl << std::endl
		<< "\\begin{document}" << std::endl;

	{ //Tests: fourth-order Fierz identities
		out << "\\section{Fourth order Fierz identities}" << std::endl;

		std::vector<std::pair<std::string, std::string>> leftIndices{
			{"i_1", "i_2"},
			{"i_2", "i_1"}
		};

		std::vector<std::pair<std::string, std::string>> rightIndices{
			{"i_1", "i_1"},
			{"i_2", "i_2"}
		};

		for (int i = 0; i < 5; ++i) {
			Identity identity;
			Expression lhs;
			lhs.terms.push_back(Expression::Term{ one<Rational>() });
			lhs.terms[0].factors.push_back(
					taggedBilinear(i, -1, false));
			lhs.terms[0].factors.push_back(
					taggedBilinear(i, -1, true));

			identity.left = lhs;
			identity.leftSpinorIndices = leftIndices;

			identity.right = lhs.fierzTransformed(0);
			identity.rightSpinorIndices = rightIndices;

			Printer prn{ "\\lambda", 3 };
			out << prn.latexify(identity) << std::endl;
		}
	}

	std::vector<Expression> hexaBasis;
	hexaBasis.resize(11);
	for (int i = 0; i < 5; ++i) {
		Expression& expr = hexaBasis[i];
		expr.terms.emplace_back(one<Rational>());
		expr.terms[0].factors.push_back(taggedBilinear(0, -1, false));
		expr.terms[0].factors.push_back(taggedBilinear(i, -1, false));
		expr.terms[0].factors.push_back(taggedBilinear(i, -1, true));
	}

	std::vector<TensorIndex> lower{
		TensorIndex{ "\\kappa", false },
		TensorIndex{ "\\lambda", false },
		TensorIndex{ "\\mu", false },
		TensorIndex{ "\\nu", false }
	};

	std::vector<TensorIndex> upper{
		TensorIndex{ "\\kappa", true },
		TensorIndex{ "\\lambda", true },
		TensorIndex{ "\\mu", true },
		TensorIndex{ "\\nu", true }
	};

	hexaBasis[5].terms.emplace_back(one<Rational>());
	hexaBasis[5].terms[0].factors.push_back(
						Bilinear::create(4, {}));
	hexaBasis[5].terms[0].factors.push_back(
						Bilinear::create(1, { lower[0] }));
	hexaBasis[5].terms[0].factors.push_back(
						Bilinear::create(3, { upper[0] }));

	hexaBasis[6].terms.emplace_back(one<Rational>());
	hexaBasis[6].terms[0].factors.push_back(
			Bilinear::create(1, { lower[1] }));
	hexaBasis[6].terms[0].factors.push_back(
			Bilinear::create(1, { lower[2] }));
	hexaBasis[6].terms[0].factors.push_back(
			Bilinear::create(2, { upper[1], upper[2] }));

	hexaBasis[7].terms.emplace_back(one<Rational>());
	hexaBasis[7].terms[0].factors.push_back(
			Bilinear::create(3, { lower[1] }));
	hexaBasis[7].terms[0].factors.push_back(
			Bilinear::create(3, { lower[2] }));
	hexaBasis[7].terms[0].factors.push_back(
			Bilinear::create(2, { upper[1], upper[2] }));

	hexaBasis[8].terms.emplace_back(LI::TensorPolynomial<Rational>{
			LI::Tensor::create(LI::Basis::epsilon, lower)});
	hexaBasis[8].terms[0].factors.push_back(
			Bilinear::create(1, { upper[0] }));
	hexaBasis[8].terms[0].factors.push_back(
			Bilinear::create(3, { upper[1] }));
	hexaBasis[8].terms[0].factors.push_back(
			Bilinear::create(2, { upper[2], upper[3] }));

	hexaBasis[9].terms.emplace_back(LI::TensorPolynomial<Rational>{
			LI::Tensor::create(LI::Basis::epsilon, lower)});
	hexaBasis[9].terms[0].factors.push_back(Bilinear::create(4, {}));
	hexaBasis[9].terms[0].factors.push_back(
			Bilinear::create(2, { upper[0], upper[1] }));
	hexaBasis[9].terms[0].factors.push_back(
			Bilinear::create(2, { upper[2], upper[3] }));

	hexaBasis[10].terms.emplace_back(one<Rational>());
	hexaBasis[10].terms[0].factors.push_back(
			Bilinear::create(2, { lower[0], upper[1] }));
	hexaBasis[10].terms[0].factors.push_back(
			Bilinear::create(2, { lower[1], upper[2] }));
	hexaBasis[10].terms[0].factors.push_back(
			Bilinear::create(2, { lower[2], upper[0] }));

	{ //Sixth-order Fierz identities
		out << "\\section{Sixth-order Fierz identities}" << std::endl;

		std::vector<std::vector<std::pair<std::string, std::string>>>
		leftIndices{
			{ {"i_1", "i_1"}, {"i_2", "i_3"}, {"i_3", "i_2"} },
			{ {"i_1", "i_2"}, {"i_2", "i_1"}, {"i_3", "i_3"} },
			{ {"i_1", "i_2"}, {"i_2", "i_3"}, {"i_3", "i_1"} }
		};

		std::vector<std::pair<std::string, std::string>> rightIndices{
			{"i_1", "i_1"},
			{"i_2", "i_2"},
			{"i_3", "i_3"}
		};

		for (const Expression& expr : hexaBasis) {
			std::vector<Identity> identities;
			identities.resize(3);

			for (size_t i = 0; i < 3; ++i) {
				identities[i].left = expr;
				identities[i].leftSpinorIndices = leftIndices[i];
				identities[i].rightSpinorIndices = rightIndices;
			}

			identities[0].right =
					collectTerms(expr.fierzTransformed(1));

			identities[1].right =
					collectTerms(expr.fierzTransformed(0));

			identities[2].right = collectTerms(
					expr.fierzTransformed(0).fierzTransformed(1));

			for (Identity& identity: identities) {
				Printer prn{ "\\sigma", 2 };
				out << prn.latexify(identity) << std::endl;
			}
		}
	}

	out << "\\end{document}";

	return 0;
}


