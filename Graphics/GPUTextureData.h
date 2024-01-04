#pragma once

#include <vector>

#include "IGPUTextureData.h"

namespace aoe {

template<typename TByte>
class GPUTextureData : public IGPUTextureData {
public:
	size_t GetByteWidth() const override {
		return sizeof(TByte);
	}

	const void* GetBufferPointer() const override {
		return static_cast<const void*>(data_.data());
	}

	size_t GetBufferSize() const override {
		return data_.size();
	}

	GPUTextureData()
		: data_()
	{}

	GPUTextureData(std::vector<TByte> data)
		: data_(std::move(data))
	{}

	GPUTextureData(std::initializer_list<TByte> bytes)
		: data_(std::move(bytes))
	{}

private:
	const std::vector<TByte> data_;
};

} // namespace aoe