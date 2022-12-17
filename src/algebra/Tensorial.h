/*
 * Tensorial.h
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_TENSORIAL_H_
#define SRC_TENSORIAL_H_

#include <string>
#include <vector>

namespace dirac {

namespace algebra {

/**
 * Base class for all objects with indices
 */
template<typename IdType>
class TensorialBase {
public:

	struct Index {
		IdType id;
		bool isUpper = true;

		Index(const IdType& aId, bool upper)
			: id{ aId }, isUpper{ upper } {}

		Index(const Index& other) = default;
		Index(Index&& other) = default;
		Index() = delete;

		Index& operator=(const Index& other) = default;

		bool operator==(const Index& other) const {
			return (isUpper == other.isUpper) && (id == other.id);
		}
	};

	TensorialBase() = default;

	using Indices = std::vector<Index>;
	TensorialBase(const Indices& indices) : _indices{ indices } {}
	TensorialBase(const TensorialBase& other) = default;
	TensorialBase(TensorialBase&& other) = default;

	const Indices& indices() const { return _indices; }
private:
	Indices _indices;
};

template<>
class TensorialBase<std::string> {};

using Tensorial = TensorialBase<std::string>;

}

}



#endif /* SRC_TENSORIAL_H_ */
