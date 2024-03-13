#pragma once

#include "GPUEnums.h"
#include "DX11GPUDevice.h"

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

class DX11GPUBuffer {
public:
	template<typename TElement>
	static DX11GPUBuffer Create(const GPUBufferDescription& description, const TElement* data, uint32_t count);
	template<typename TElement>
	static DX11GPUBuffer Create(const GPUBufferDescription& description, uint32_t count = 1);

	DX11GPUBuffer(GPUBufferDescription description, const void* data, uint32_t size, uint32_t stride);

	ID3D11Buffer* GetNative() const;
	const GPUBufferDescription& GetDescription() const;

	uint32_t GetElementsCount() const;
	uint32_t GetSize() const;
	uint32_t GetStride() const;

	bool IsVertexBuffer() const;
	bool IsIndexBuffer() const;
	bool IsConstantBuffer() const;
	bool IsDynamic() const;

private:
	GPUBufferDescription description_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;

	uint32_t size_;
	uint32_t stride_;

	static D3D11_USAGE ToDXUsage(const GPUResourceUsage value);
	static D3D11_BIND_FLAG ToDXBufferType(const GPUBufferType buffer_type);
};

template<typename TElement>
inline DX11GPUBuffer DX11GPUBuffer::Create(const GPUBufferDescription& description, const TElement* data, uint32_t count) {
	uint32_t stride = sizeof(TElement);
	uint32_t size = stride * count;
	return { description, data, size, stride };
}

template<typename TElement>
inline DX11GPUBuffer DX11GPUBuffer::Create(const GPUBufferDescription& description, uint32_t count) {
	return Create<TElement>(description, nullptr, count);
}

} // namespace aoe