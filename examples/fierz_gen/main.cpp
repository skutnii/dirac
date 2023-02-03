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

	{ //Sixth-order Fierz identities
		out << "\\section{Sixth-order Fierz identities}" << std::endl;

		for (size_t i = 0; i < hexaBasis.size(); ++i) {
			const Expression& expr = hexaBasis[i];
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
					expr.fierzTransformed(1).fierzTransformed(0));

			for (Identity& identity: identities) {
				Printer prn{ "\\sigma", 2 };
				out << prn.latexify(identity) << std::endl;
			}
		}
	}

	{
		out << "\\section{Simplification tests}" << std::endl;

		std::vector<Identity> tests;

		tests.emplace_back();
		tests[0].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[0].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[1], lower[2]}));
		tests[0].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[0].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[1], lower[2]}));
		tests[0].leftSpinorIndices = rightIndices;
		tests[0].rightSpinorIndices = rightIndices;

		tests.emplace_back();
		tests[1].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[1].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[1], lower[2]}));
		tests[1].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[1].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[2], lower[1]}));
		tests[1].leftSpinorIndices = rightIndices;
		tests[1].rightSpinorIndices = rightIndices;

		tests.emplace_back();
		tests[2].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[2].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[1], lower[2]}));
		tests[2].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[1]}));

		tests[2].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[2].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[0], lower[2]}));
		tests[2].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[0]}));
		tests[2].leftSpinorIndices = rightIndices;
		tests[2].rightSpinorIndices = rightIndices;

		tests.emplace_back();
		tests[3].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[3].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[1], lower[2]}));
		tests[3].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[1]}));

		tests[3].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[3].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[2], lower[0]}));
		tests[3].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[0]}));
		tests[3].leftSpinorIndices = rightIndices;
		tests[3].rightSpinorIndices = rightIndices;

		tests.emplace_back();
		tests[4].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[4].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[1], lower[2]}));
		tests[4].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[1]}));

		tests[4].left.terms.emplace_back(
				LI::TensorPolynomial<Rational>{ one<Rational>() });
		tests[4].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[0]}));
		tests[4].left.terms.back().factors.push_back(
				Bilinear::create(2, {lower[2], lower[0]}));
		tests[4].leftSpinorIndices = rightIndices;
		tests[4].rightSpinorIndices = rightIndices;

		tests.emplace_back();
		tests[5].left.terms.emplace_back(
				LI::Tensor::create(LI::Basis::eta,
								{ lower[0], lower[1] }));
		tests[5].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[0]}));
		tests[5].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[1]}));

		tests[5].left.terms.emplace_back(
				LI::Tensor::create(LI::Basis::eta,
								{ lower[1], lower[2] }));
		tests[5].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[1]}));
		tests[5].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[2]}));
		tests[5].leftSpinorIndices = rightIndices;
		tests[5].rightSpinorIndices = rightIndices;

		tests.emplace_back();
		tests[6].left.terms.emplace_back(
				LI::Tensor::create(LI::Basis::eta,
								{ lower[0], lower[1] }));
		tests[6].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[0]}));
		tests[6].left.terms.back().factors.push_back(
				Bilinear::create(1, {upper[1]}));

		tests[6].left.terms.emplace_back(
				LI::Tensor::create(LI::Basis::eta,
								{ lower[1], lower[2] }));
		tests[6].left.terms.back().factors.push_back(
				Bilinear::create(3, {upper[1]}));
		tests[6].left.terms.back().factors.push_back(
				Bilinear::create(3, {upper[2]}));
		tests[6].leftSpinorIndices = rightIndices;
		tests[6].rightSpinorIndices = rightIndices;

		for (Identity& test : tests) {
			test.right = collectTerms(test.left);
			Printer prn{ "\\sigma", 3 };
			out << prn.latexify(test) << std::endl;
		}
	}

	out << "\\end{document}";

	return 0;
}


