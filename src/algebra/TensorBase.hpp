/*
 * TensorBase.hpp
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
#include <limits>

#include "concepts.hpp"
#include "Tensorial.hpp"

#include "Permutations.hpp"

namespace dirac {

namespace algebra {

/**
 * Requirements for a tensor ring basis
 */
template<typename T, typename IdType>
concept TensorBasis = requires(const IdType& id) {
	{ T::allows(id) } -> std::same_as<bool>;
	{ T::maxIndexCount(id) } -> std::same_as<size_t>;
};

/**
 * Base element of a tensor ring.
 * The first template argument is the type
 * used for element identifiers,
 * the second one is the ring basis definition,
 * the third one is tensor index identifier type
 */
template<Hashable IdType,
			TensorBasis<IdType> Basis,
			typename IndexIdType>
class TensorBase {
public:
	using Self = TensorBase<IdType, Basis, IndexIdType>;
	using Index = IndexBase<IndexIdType>;
	using Indices = std::vector<Index>;

	TensorBase() = delete;
	TensorBase(const Self& other) = default;
	TensorBase(Self&& other) = default;

	Self& operator=(const Self& other) = default;

	/**
	 * Element identifier
	 */
	const IdType& id() const { return _id; }

	/**
	 * Element indices
	 */
	const Indices& indices() const { return _indices; };

	/**
	 * Maximum number of indices the element can have
	 */
	unsigned int maxIndices() const { return _maxIndices; };

	/**
	 * Actual number of indices the tensor has
	 */
	unsigned int actualIndexCount() const { return _indices.size(); }

	/**
	 * The tensor is considered complete
	 * if it has maximum number of indices.
	 */
	bool complete() const {
		return (actualIndexCount() >= _maxIndices);
	}

	/**
	 * Construct a new instance by appending an index to the end
	 */
	Self operator^(const TensorIndex& idx) {
		guardMaxIndices(1);

		Self res{ *this };
		res._indices.push_back(idx);

		return res;
	}

	/**
	 * Construct a new instance by appending
	 * the list of indices to the end
	 */
	Self operator^(const Indices& indices) {
		guardMaxIndices(indices.size());


		Self res{ *this };
		res._indices.insert(res._indices.end(),
				indices.begin(), indices.end());

		return res;
	}

	/**
	 * Creates a tensor from identifier and indices
	 */
	static Self create(const IdType& id, const Indices& indices = {}) {
		if (!Basis::allows(id))
			throw std::runtime_error{
					"Tensor identifier not in basis" };

		return Self{ id, indices };
	}

	/**
	 * Creates a tensor from an identifier and index
	 */
	static Self makeWithIndex(const IdType& id,
			const IndexIdType& index,
			bool isUpper) {
		if (!Basis::allows(id))
			throw std::runtime_error{ "Tensor identifier not in basis" };

		if (Basis::maxIndexCount(id) < 1)
			throw std::runtime_error{ "Subscript operation forbidden" };

		Indices tmpIndices;
		tmpIndices.emplace_back(index, isUpper);
		return Self{ id, tmpIndices };
	}

	/**
	 * Creates a tensor from an identifier
	 * and an abstract sequence of indices.
	 */
	template<std::input_iterator IBegin,
	std::sized_sentinel_for<IBegin> IEnd>
	static TensorBase makeWithIndices(const IdType& id,
			const IBegin& indexStart,
			const IEnd& indexEnd,
			bool isUpper) {

		if (!Basis{}.allows(id))
			throw std::runtime_error{ "Tensor identifier not in basis" };

		size_t idxCount = indexEnd - indexStart;
		if (idxCount > Basis::maxIndexCount(id))
			throw std::runtime_error{ "Too many tensor indices" };

		Indices tmp;
		tmp.reserve(idxCount);
		for (IBegin it = indexStart; it != indexEnd; ++it)
			tmp.emplace_back(*it, isUpper);

		return TensorBase{ id, tmp };
	}

	/**
	 * Equality check. Two tensors are equal
	 * if they have the same identifier and indices
	 */
	bool operator==(
			const TensorBase<IdType, Basis, IndexIdType>& other) const {
		if (_id != other._id)
			return false;

		if (_indices.size() != other._indices.size())
			return false;

		for (size_t i = 0; i < _indices.size(); ++i)
			if (_indices[i] != other._indices[i])
				return false;

		return true;
	}

	/**
	 * Replaces the index at position specified by the first argument
	 * with the second argument. Exception is thrown on range error.
	 */
	void replaceIndex(size_t pos, const Index& repl) {
		_indices.at(pos) = repl;
	}

	/**
	 * If the callee is mappable to the argument
	 * by an index permutation,
	 * returns the latter or an empty optional otherwise.
	 */
	std::optional<Permutation>
	mappingTo(
			const TensorBase<IdType, Basis, IndexIdType>& other) const {
		std::optional<Permutation> res;
		if ((_id != other._id)
				|| (_indices.size() != other._indices.size()))
			return res;

		forPermutations(_indices.size(),
			[&](const Permutation& perm) {
				if (res)
					return;

				bool match = true;
				for (size_t i = 0; i < _indices.size(); ++i)
					if (_indices[perm.map[i]] != other._indices[i])
						match = false;

				if (match)
					res = perm;
			});

		return res;
	}

private:
	explicit TensorBase(const IdType& id,
			const Indices& indices = {})
	: _id{ id },
	  _indices{ indices },
	  _maxIndices{ Basis::maxIndexCount(id) } {}

	/**
	 * Checks whether actual index count + the argument is lesser than
	 * or equal to the maximum index count an throws an eror otherwise.
	 * This method is used to prevent construction of tensors
	 * with excess number of indices.
	 *
	 * On the other hand, incomplete tensors are allowed
	 * as they may be produced by subscript and superscript operations.
	 */
	void guardMaxIndices(size_t extraCount) const {
		if ((_indices.size() + extraCount) > _maxIndices)
			throw std::runtime_error{
			"Reached maximum number of indices" };
	}

	IdType _id;
	Indices _indices;
	size_t _maxIndices = 0;
};



}

}

#endif /* SRC_TENSOR_H_ */
