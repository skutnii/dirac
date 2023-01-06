/*
 * App.cpp
 *
 *  Created on: Jan 6, 2023
 *      Author: skutnii
 */

#include "App.hpp"
#include "algebra/Rational.hpp"
#include <iostream>

namespace dirac {

static const std::string exprOption{ "-e" };
static const std::string modeOption{ "-m" };
static const std::string lineTermsOption{ "-l" };
static const std::string dummyNameOption{ "-d" };
static const std::string applySymmetryOption{ "-s" };

//----------------------------------------------------------------------

App::App(int argc, char **argv) {
	enum Option {
		None,
		Expr,
		Mode,
		LineTerms,
		DummyName,
		ApplySymmetry
	};

	Option expectedOption = None;
	for (int argNum = 1; argNum < argc; ++argNum) {
		char* arg = argv[argNum];

		//Process option keys

		if (exprOption == arg) {
			expectedOption = Expr;
			continue;
		}

		if (modeOption == arg) {
			expectedOption = Mode;
			continue;
		}

		if (lineTermsOption == arg) {
			expectedOption = LineTerms;
			continue;
		}

		if (dummyNameOption == arg) {
			expectedOption = DummyName;
			continue;
		}

		if (applySymmetryOption == arg) {
			expectedOption = ApplySymmetry;
			continue;
		}

		//Process option value

		switch(expectedOption) {
		case Expr:
			_commandLineExpr = arg;
			break;
		case Mode: {
			std::optional<bool> maybeValue = getUseFloat(arg);
			if (maybeValue.has_value())
				_useFloat = maybeValue.value();
			break;
		}
		case LineTerms: {
			std::optional<size_t> maybeTerms = getLineTerms(arg);
			if (maybeTerms.has_value())
				_lineTerms = maybeTerms.value();
			break;
		}
		case DummyName:
			_dummyName = arg;
			break;
		case ApplySymmetry: {
			std::optional<bool> maybeValue = getBoolean(arg);
			if (maybeValue.has_value())
				_applySymmetry = maybeValue.value();
			break;
		}
		default:
			break;
		};

		expectedOption = None;
	}
}

//----------------------------------------------------------------------

void App::setVar(const std::string& name, const std::string& value) {
	if (name == "mode") {
		std::optional<bool> maybeValue = getUseFloat(value);
		if (maybeValue.has_value())
			_useFloat = maybeValue.value();
		else
			std::cout
				<< "Invalid mode. Must be \"float\" or \"rational\""
				<< std::endl;

		return;
	}

	if (name == "line_terms") {
		std::optional<size_t> maybeTerms = getLineTerms(value);
		if (maybeTerms.has_value()) {
			_lineTerms = maybeTerms.value();
		} else
			std::cout
				<< "Invalid line terms count."
					" Must be an integer constant or \"inf\""
				<< std::endl;

		return;
	}

	if (name == "dummy") {
		_dummyName = value;
		return;
	}

	if (name == "apply_symmetry") {
		std::optional<bool> maybeValue = getBoolean(value);
		if (maybeValue.has_value())
			_applySymmetry = maybeValue.value();
		else
			std::cout
				<< "Invalid boolean literal. "
				   "Must be \"true\" or \"false\"" << std::endl;
		return;
	}

	std::cout << "Unknown variable name " << name << std::endl;
}

//----------------------------------------------------------------------

int App::run() {
	if (_commandLineExpr.empty())
		return runShell();

	return compute<void>(_commandLineExpr, std::cout);
}

//----------------------------------------------------------------------

template<>
int App::compute<void>(const std::string& input,
		std::ostream& output) const noexcept {
	if (_useFloat)
		return compute<double>(input, output);
	else
		return compute<algebra::Rational>(input, output);
}

//----------------------------------------------------------------------

std::optional<bool> App::getUseFloat(const std::string &value) {
	if (value == "float")
		return true;
	else if (value == "rational")
		return false;

	return std::optional<bool>{};
}

//----------------------------------------------------------------------

std::optional<size_t> App::getLineTerms(const std::string &str) {
	try {
		if (str == "inf")
			return 0;

		size_t num_chars = std::numeric_limits<size_t>::max();
		size_t count = std::stoll(str, &num_chars);
		if (num_chars < str.size())
			return std::optional<size_t>{};

		return count;
	} catch(...) {
		return std::optional<size_t>{};
	}
}

//----------------------------------------------------------------------

std::optional<bool> App::getBoolean(const std::string &str) {
	if (str == "true")
		return true;

	if (str == "false")
		return false;

	return std::optional<bool>{};
}

//----------------------------------------------------------------------

int App::runShell() {
	std::cout <<
			"This is Dirac matrices calculator by Sergii Kutnii"
			<< std::endl;

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

		compute<void>(input, std::cout);
	}

	return 0;
}

} /* namespace dirac */
