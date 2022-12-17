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
};

using TensorIndex = IndexBase<IndexId>;
using TensorIndices = std::vector<TensorIndex>;

}

}



#endif /* SRC_TENSORIAL_H_ */
