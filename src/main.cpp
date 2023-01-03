/*
 * main.cpp
 *
 *  Created on: 11 Dec. 2022 р.
 *      Author: skutnii
 */

#include <iostream>

#include "eval.hpp"
#include "ExprPrinter.hpp"
#include "utils.hpp"

using namespace dirac;
using namespace symbolic;

template<typename Number>
void process(const std::string& input, std::ostream& output) {
	try {
		CanonicalExpr<Number> expr = eval<Number>(input);
		ExprPrinter<Number> printer{ "\\omega" };
		output << printer.latexify(expr) << std::endl;
	} catch (std::exception& e) {
		output << e.what() << std::endl;
	}
}

bool useFloat = false;

int main(int argc, char **argv) {
	std::cout << "This is Dirac matrices calculator by Sergii Kutnii" << std::endl;

	//Read-eval-print loop
	std::string input;
	std::string prompt{"dirac:> "};
	while(true) {
		std::cout << prompt;
		std::getline(std::cin, input);

		std::vector<std::string> words = utils::get_words(input);
		if ((words.size() == 1) && (words[0] == "quit"))
			break;

		if (words.empty())
			continue;

		if ((words.size() == 2) && (words[0] == "#use")) {
			if (words[1] == "float")
				useFloat = true;
			else if (words[1] == "rational")
				useFloat = false;
			else
				std::cout
					<< "Invalid use-expression. Should be \"#use float\" or \"#use rational\""
					<< std::endl;

			continue;
		}

		if (useFloat)
			process<double>(input, std::cout);
		else
			process<algebra::Rational>(input, std::cout);
	}

	return 0;
}


