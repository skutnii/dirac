/*
 * eval.h
 *
 *  Created on: Dec 11, 2022
 *      Author: skutnii
 */

#ifndef EVAL_H_
#define EVAL_H_

#include <string>
#include <ostream>
#include "Symbolic.hpp"

namespace dirac {

/**
 * Main evaluation routine
 */
symbolic::CanonicalExpr eval(const std::string& expr);

}

#endif /* EVAL_H_ */
