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

int main(int argc, char **argv) {
	std::cout << "This is Dirac matrices calculator by Sergii Kutnii" << std::endl;

	std::string input;
	std::string prompt{"dirac:> "};
	while(true) {
		std::cout << prompt;
		std::getline(std::cin, input);
		if (input == "quit")
			break;

		if (input.empty())
			continue;

		try {
			CanonicalExpr expr = eval(input);
			ExprPrinter printer{ "\\omega" };
			std::cout << printer.latexify(expr);
		} catch (std::exception& e) {
			std::cout << e.what() << std::endl;
		}
	}

	return 0;
}


