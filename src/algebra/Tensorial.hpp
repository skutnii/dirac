/*
 * Tensorial.hpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_TENSORIAL_H_
#define SRC_TENSORIAL_H_

#include <string>
#include <vector>
#include "IndexId.hpp"

namespace dirac {

namespace algebra {

template<Hashable IdType>
struct IndexBase {
	IdType id;
	bool isUpper = true;

	IndexBase(const IdType& aId, bool upper)
		: id{ aId }, isUpper{ upper } {}

	IndexBase(const IndexBase& other) = default;
	IndexBase(IndexBase&& other) = default;
	IndexBase() = delete;

	IndexBase& operator=(const IndexBase& other) = default;

	bool operator==(const IndexBase& other) const {
		return (isUpper == other.isUpper) && (id == other.id);
	}

	bool dual(const IndexBase<IdType>& other) const {
		return (id == other.id) && (isUpper == !other.isUpper);
	}
};

using TensorIndex = IndexBase<IndexId>;
using TensorIndices = std::vector<TensorIndex>;

}

}

namespace std {

template<dirac::Hashable IdType>
struct hash<dirac::algebra::IndexBase<IdType>> {
	size_t operator()(const dirac::algebra::IndexBase<IdType>& idx) const {
		return std::hash<IdType>{}(idx.id) ^ static_cast<size_t>(idx.isUpper);
	}
};

}



#endif /* SRC_TENSORIAL_H_ */
