/*
 * Tensor.h
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#ifndef SRC_TENSOR_H_
#define SRC_TENSOR_H_

#include <vector>
#include <string>
#include <unordered_set>
#include <list>
#include <optional>
#include <memory>

#include "concepts.hpp"
#include "Tensorial.h"

namespace dirac {

namespace algebra {

template<typename IdType,
			Restriction<IdType> Basis,
			typename IndexIdType = IdType>
class TensorBase : public TensorialBase<IndexIdType> {
public:
	TensorBase() = delete;
	TensorBase(const TensorBase& other) = default;
	TensorBase(TensorBase&& other) = default;

	static size_t maxIndexCount(const IdType& id);

	static TensorBase fromId(const IdType& id) {
		if (!Basis{}.allows(id))
			throw std::runtime_error{ "Tensor identifier not in basis" };

		return TensorBase{ id };
	}

	TensorBase& operator=(const TensorBase& other) = default;

	const IdType& id() const { return _id; }

	unsigned int maxIndices() const { return _maxIndices; };
	unsigned int actualIndexCount() const { return Base::indices().size(); }
	bool complete() const { return (actualIndexCount() >= _maxIndices); }

	using Base = TensorialBase<IndexIdType>;
	using Index = typename Base::Index;
	using Indices = typename Base::Indices;

	//Construct a new instance by appending an index to the end
	TensorBase operator^(const Index& idx) {
		guardMaxIndices(1);

		Indices tmpIndices = Base::indices();
		tmpIndices.push_back(idx);

		return TensorBase{ _id, tmpIndices };
	}

	TensorBase operator^(const Indices& indices) {
		guardMaxIndices(indices.size());

		Indices tmpIndices = indices();
		tmpIndices.insert(tmpIndices.end(), indices.begin(), indices.end());

		return TensorBase{ _id, tmpIndices };
	}

	static TensorBase makeWithIndex(const IdType& id,
			const IndexIdType& index,
			bool isUpper) {
		if (!Basis{}.allows(id))
			throw std::runtime_error{ "Tensor identifier not in basis" };

		if (maxIndexCount(id) < 1)
			throw std::runtime_error{ "Subscript operation forbidden" };

		Indices tmpIndices;
		tmpIndices.emplace_back(index, isUpper);
		return TensorBase{ id, tmpIndices };
	}

	template<std::input_iterator IBegin, std::sized_sentinel_for<IBegin> IEnd>
	static TensorBase makeWithIndices(const IdType& id,
			const IBegin& indexStart,
			const IEnd& indexEnd,
			bool isUpper) {

		if (!Basis{}.allows(id))
			throw std::runtime_error{ "Tensor identifier not in basis" };

		size_t idxCount = indexEnd - indexStart;
		if (idxCount > maxIndexCount(id))
			throw std::runtime_error{ "Too many tensor indices" };

		Indices tmp;
		tmp.reserve(idxCount);
		for (IBegin it = indexStart; it != indexEnd; ++it)
			tmp.emplace_back(*it, isUpper);

		return TensorBase{ id, tmp };
	}

private:
	explicit TensorBase(const IdType& id,
			const Indices& indices)
	: Base{ indices },
	  _id{ id },
	  _maxIndices{ maxIndexCount(id) } {}

	void guardMaxIndices(size_t extraCount) const {
		if ((Base::indices().size() + extraCount) > _maxIndices)
			throw std::runtime_error{ "Reached maximum number of indices" };
	}

	IdType _id;
	size_t _maxIndices = 0;
};



}

}

#endif /* SRC_TENSOR_H_ */
