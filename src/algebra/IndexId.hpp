/*
 * IndexId.hpp
 *
 * Tensor index identifier type definition and hash function
 *
 *  Created on: Dec 17, 2022
 *      Author: skutnii
 */

#ifndef SRC_ALGEBRA_INDEXID_HPP_
#define SRC_ALGEBRA_INDEXID_HPP_

/*
 * IndexId.hpp
 *
 *  Created on: Dec 17, 2022
 *      Author: skutnii
 */

#include <utility>
#include <variant>
#include <string>

namespace dirac {

namespace algebra {

/**
 * Tensor index identifier type definition
 */
using IndexTag = std::pair<int, int>;
using IndexId = std::variant<std::string, IndexTag>;

} /* namespace dirac */

} /* namespace algebra */

namespace std {

template<>
struct hash<dirac::algebra::IndexTag> {
	size_t operator()(const dirac::algebra::IndexTag& aId) const {
		return static_cast<size_t>(aId.first ^ aId.second);
	}
};

}

#endif /* SRC_ALGEBRA_INDEXID_HPP_ */
