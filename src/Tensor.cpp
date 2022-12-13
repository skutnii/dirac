/*
 * Tensor.cpp
 *
 *  Created on: Dec 13, 2022
 *      Author: skutnii
 */

#include "Tensor.h"
#include <stdexcept>
#include <algorithm>

namespace dirac {

namespace algebra {

const std::string Tensor::eta{ "\\eta" };
const std::string Tensor::epsilon{ "\\epsilon" };
const std::string Tensor::delta{ "\\delta" };

const Tensor::NameSet Tensor::BasisTensorNames{
	eta,
	delta,
	epsilon
};

TensorPtr Tensor::fromName(const std::string &name) {
	if (BasisTensorNames.find(name) == BasisTensorNames.end())
		throw std::runtime_error{ "Unknown tensor name " + name };

	return TensorPtr{ new Tensor{name} };
}

TensorPtr Tensor::subscript(const std::string& name, const std::string& index) {
	return Tensor::fromName(name) ^ Index{ index, false };
}

TensorPtr Tensor::subscript(const std::string& name, const IndexList& indices) {
	TensorPtr instance = fromName(name);
	instance->guardMaxIndices(indices.size());

	std::transform(indices.begin(), indices.end(),
			instance->_indices.begin(),
			[](const std::string& iName) -> Index {
				return Index{ iName, false };
			});

	instance->_actualIndices += indices.size();

	return instance;
}

TensorPtr Tensor::superscript(const std::string &name, const std::string& index) {
	return fromName(name) ^ Index{ index, true };
}

TensorPtr Tensor::superscript(const std::string &name, const IndexList& indices) {
	TensorPtr instance = fromName(name);
	instance->guardMaxIndices(indices.size());

	std::transform(indices.begin(), indices.end(),
			instance->_indices.begin(),
			[](const std::string& iName) -> Index {
				return Index{ iName, true };
			});

	instance->_actualIndices += indices.size();

	return instance;
}

void Tensor::guardMaxIndices(size_t extraCount) const {
	if ((_indices.size() + extraCount) > _maxIndices)
		throw std::runtime_error{ "Reached maximum number of indices for " + _name };

}

Tensor Tensor::operator^(const Index& idx) {
	guardMaxIndices(1);

	Tensor clone{ *this };
	clone._indices[_actualIndices] = idx;
	clone._actualIndices++;
	return clone;
}

Tensor Tensor::operator^(const std::vector<Index>& indices) {
	size_t extraCount = indices.size();
	guardMaxIndices(extraCount);

	Tensor clone{ *this };

	for (size_t i = 0; i < extraCount; ++i)
		clone._indices[clone._actualIndices + i] = indices[i];

	clone._actualIndices += indices.size();
	return clone;
}

Tensor::Tensor(const std::string &n) : _name{n} {
	if ((eta == _name) || (delta == _name))
		_maxIndices = 2;
	else if (epsilon == _name)
		_maxIndices = 4;

	_indices.resize(_maxIndices);
}

TensorPtr operator^(const TensorPtr &t, const Tensor::Index &idx) {
	t->guardMaxIndices(1);

	TensorPtr clone = std::make_shared<Tensor>(*t);
	clone->_indices[clone->_actualIndices] = idx;
	clone->_actualIndices++;
	return clone;
}

TensorPtr operator^(const TensorPtr &t,
		const std::vector<Tensor::Index> &indices) {
	size_t extraCount = indices.size();
	t->guardMaxIndices(extraCount);

	TensorPtr clone = std::make_shared<Tensor>(*t);

	for (size_t i = 0; i < extraCount; ++i)
		clone->_indices[clone->_actualIndices + i] = indices[i];

	clone->_actualIndices += indices.size();
	return clone;
}

} //Namespace algebra

} //namespace dirac


