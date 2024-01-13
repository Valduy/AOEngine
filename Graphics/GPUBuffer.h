#pragma once

#include <vector>

#include "GPUDevice.h"

namespace aoe {

struct GPUBufferDescription {
	GPUBufferType buffer_type;
	GPUResourceUsage usage;

	bool IsVertexBuffer() const {
		return buffer_type == GPUBufferType::kVertexBuffer;
	}

	bool IsIndexBuffer() const {
		return buffer_type == GPUBufferType::kIndexBuffer;
	}

	bool IsConstantBuffer() const {
		return buffer_type == GPUBufferType::kConstantBuffer;
	}

	bool IsDynamic() const {
		return usage == GPUResourceUsage::kDynamic;
	}

	GPUBufferDescription(GPUBufferType buffer_type, GPUResourceUsage usage)
		: buffer_type(buffer_type)
		, usage(usage)
	{}

	GPUBufferDescription()
		: buffer_type(GPUBufferType::kVertexBuffer)
		, usage(GPUResourceUsage::kDefault)
	{}
};

class GPUBuffer {
public:
	template<typename TElement>
	static GPUBuffer Create(const GPUDevice& device, GPUBufferDescription description, const TElement* data, size_t count);

	template<typename TElement>
	static GPUBuffer Create(const GPUDevice& device, GPUBufferDescription description, size_t count = 1);

	ID3D11Buffer* GetNative() const;
	const GPUBufferDescription& GetDescription() const;

	size_t GetElementsCount() const;
	size_t GetSize() const;
	size_t GetStride() const;

	bool IsVertexBuffer() const;
	bool IsIndexBuffer() const;
	bool IsConstantBuffer() const;
	bool IsDynamic() const;

	GPUBuffer(const GPUDevice& device, GPUBufferDescription description, const void* data, size_t size, size_t stride);

private:
	const GPUDevice& device_;
	GPUBufferDescription description_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;

	size_t size_;
	size_t stride_;

	static D3D11_USAGE ToDXUsage(const GPUResourceUsage value);
	static D3D11_BIND_FLAG ToDXBufferType(const GPUBufferType buffer_type);
};

template<typename TElement>
GPUBuffer GPUBuffer::Create(const GPUDevice& device, GPUBufferDescription description, const TElement* data, size_t count) {
	return { device, description, data, sizeof(TElement) * count, sizeof(TElement) };
}

template<typename TElement>
GPUBuffer GPUBuffer::Create(const GPUDevice& device, GPUBufferDescription description, size_t count) {
	return { device, description, nullptr, sizeof(TElement) * count, sizeof(TElement) };
}

} // namespace aoe