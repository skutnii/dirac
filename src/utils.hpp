/*
 * utils.hpp
 *
 * Utility functions
 *
 *  Created on: Jan 4, 2023
 *      Author: skutnii
 */

#ifndef SRC_UTILS_HPP_
#define SRC_UTILS_HPP_

#include <vector>
#include <string>

namespace dirac {

namespace utils {

/**
 * Gets  words i.e. space-separated parts from a string
 */
std::vector<std::string> get_words(const std::string& str);

}

}



#endif /* SRC_UTILS_HPP_ */
