/*
 * compute.h
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#ifndef EVAL_H_
#define EVAL_H_

#include <string>
#include <ostream>

namespace dirac {

/**
 * Main evaluation routine
 */
void eval(const std::string& expr, std::ostream& output);

}

#endif /* EVAL_H_ */
