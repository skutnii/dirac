/*
 * App.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: skutnii
 */

#include "App.hpp"

#include <iostream>
#include "eval.hpp"
#include "ExprPrinter.hpp"
#include "utils.hpp"

namespace dirac {

using namespace symbolic;

App::App(int argc, char **argv) {
}

void App::setVar(const std::string &name, const std::string &value) {
	if (name == "mode") {
		if (value == "float")
			_useFloat = true;
		else if (value == "rational")
			_useFloat = false;
		else
			std::cout
				<< "Invalid mode. Must be \"float\" or \"rational\""
				<< std::endl;

		return;
	}

	std::cout << "Unknown variable name " << name << std::endl;
}

template<typename Number>
static int process(const std::string& input, std::ostream& output) {
	try {
		CanonicalExpr<Number> expr = eval<Number>(input);
		ExprPrinter<Number> printer{ "\\omega" };
		output << printer.latexify(expr) << std::endl;
		return 0;
	} catch (std::exception& e) {
		output << e.what() << std::endl;
		return 1;
	}
}

int App::run() {
	if (_commandLineExpr.empty())
		return runShell();

	return compute(_commandLineExpr);
}

int App::compute(const std::string &expr) const {
	if (_useFloat)
		return process<double>(expr, std::cout);
	else
		return process<algebra::Rational>(expr, std::cout);
}

int App::runShell() {
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

		if ((words.size() == 3) && (words[0] == "#set")) {
			setVar(words[1], words[2]);
			continue;
		}

		compute(input);
	}

	return 0;
}

} /* namespace dirac */
