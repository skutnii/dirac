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

namespace dirac {

namespace algebra {

class Tensor;
using TensorPtr = std::shared_ptr<Tensor>;

class Tensor {
public:
	struct Index {
		std::string id;
		bool isUpper = true;

		Index(const std::string& aId, bool upper)
			: id{ aId }, isUpper{ upper } {}

		Index(const Index& other) = default;
		Index(Index&& other) = default;
		Index() = delete;

		Index& operator=(const Index& other) = default;

		bool operator==(const Index& other) const {
			return (isUpper == other.isUpper) && (id == other.id);
		}
	};

	Tensor() = delete;
	Tensor(const Tensor& other) = default;
	Tensor(Tensor&& other) = default;

	static const std::string eta;
	static const std::string epsilon;
	static const std::string delta;

	using NameSet = std::unordered_set<std::string>;
	static const NameSet BasisTensorNames;

	static TensorPtr fromName(const std::string& name);

	Tensor& operator=(const Tensor& other) = default;

	const std::string& name() const { return _name; }

	using Indices = std::vector<std::optional<Index>>;
	const Indices indices() const { return _indices; }

	unsigned int maxIndices() const { return _maxIndices; };
	unsigned int actualIndexCount() const { return _actualIndices; }

	using IndexList = std::list<std::string>;

	//Construct a new instance by appending an index to the end
	Tensor operator^(const Index& idx);
	Tensor operator^(const std::vector<Index>& indices);

	friend TensorPtr operator^(const TensorPtr& t, const Index& idx);
	friend TensorPtr operator^(const TensorPtr& t, const std::vector<Index>& indices);

	static TensorPtr subscript(const std::string& name, const std::string& index);
	static TensorPtr subscript(const std::string& name, const IndexList& indices);

	static TensorPtr superscript(const std::string& name, const std::string& index);
	static TensorPtr superscript(const std::string& name, const IndexList& indices);

private:
	explicit Tensor(const std::string& n);

	void guardMaxIndices(size_t extraCount) const;

	std::string _name;
	Indices _indices;
	unsigned int _maxIndices = 0;
	unsigned int _actualIndices = 0;
};

TensorPtr operator^(const TensorPtr& t, const Tensor::Index& idx);
TensorPtr operator^(const TensorPtr& t, const std::vector<Tensor::Index>& indices);

}

}

#endif /* SRC_TENSOR_H_ */
