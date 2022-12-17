/*
 * main.cpp
 *
 *  Created on: 11 Dec. 2022 р.
 *      Author: skutnii
 */

#include <iostream>
#include "eval.h"

using namespace dirac;

int main(int argc, char **argv) {
	std::cout << "This is Dirac matrices calculator by Sergii Kutnii" << std::endl;

	std::string input;
	std::string prompt{"dirac:> "};
	while(true) {
		std::cout << prompt;
		std::getline(std::cin, input);
		if (input == "quit")
			break;

		if (!input.empty())
			eval(input, std::cout);
	}

	return 0;
}


