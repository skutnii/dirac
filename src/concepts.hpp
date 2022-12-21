/*
 * concepts.hpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_CONCEPTS_HPP_
#define SRC_CONCEPTS_HPP_

#include <unordered_set>
#include <type_traits>

namespace dirac {

template<typename T>
concept Hashable = requires(T a) {
    { std::hash<T>{}(a) } -> std::convertible_to<std::size_t>;
};

}

#endif /* SRC_CONCEPTS_HPP_ */
