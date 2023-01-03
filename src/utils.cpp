/*
 * utils.cpp
 *
 *  Created on: Jan 4, 2023
 *      Author: skutnii
 */

#include "utils.hpp"

namespace dirac {

namespace utils {

std::vector<std::string> get_words(const std::string &str) {
	std::vector<std::string> words;
	bool isSpace = true;
	for (char c : str) {
		bool nextIsSpace = isspace(c);
		if (!nextIsSpace) {
			if (isSpace)
				words.emplace_back();

			words.back().push_back(c);
		}

		isSpace = nextIsSpace;
	}

	return words;
}


}

}


