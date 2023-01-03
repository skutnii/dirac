/*
 * main.cpp
 *
 *  Created on: 11 Dec. 2022 р.
 *      Author: skutnii
 */

#include <iostream>

#include "eval.hpp"
#include "ExprPrinter.hpp"

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

int main(int argc, char **argv) {
	std::cout << "This is Dirac matrices calculator by Sergii Kutnii" << std::endl;

	//Read-eval-print loop
	std::string input;
	std::string prompt{"dirac:> "};
	while(true) {
		std::cout << prompt;
		std::getline(std::cin, input);
		if (input == "quit")
			break;

		if (input.empty())
			continue;

		process<algebra::Rational>(input, std::cout);
	}

	return 0;
}


