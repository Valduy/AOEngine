#pragma once

#include <vector>

#include "GPUDevice.h"

namespace aoe {

class GPUDevice;

struct GPUBufferDescription {
	GPUBufferType buffer_type;
	const void* data;
	size_t size;
	size_t stride;
	GPUResourceUsage usage;

	size_t GetElementsCount() const {
		return stride > 0 ? size / stride : 0;
	}

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

	GPUBufferDescription()
		: GPUBufferDescription(GPUBufferType::kVertexBuffer, nullptr, 0, 0, GPUResourceUsage::kDefault)
	{}

	GPUBufferDescription(GPUBufferType buffer_type, const void* data, size_t size, size_t stride, GPUResourceUsage usage)
		: buffer_type(buffer_type)
		, data(data)
		, size(size)
		, stride(stride)
		, usage(usage)
	{}

	template<typename TElement>
	static GPUBufferDescription Create(
		GPUBufferType type, 
		const TElement* data, 
		size_t count, 
		GPUResourceUsage usage) 
	{
		size_t stride = sizeof(TElement);
		return { data, stride * count, stride, usage };
	}

	template<typename TElement>
	static GPUBufferDescription CreateVertex(
		const TElement* data, 
		size_t count, 
		GPUResourceUsage usage = GPUResourceUsage::kDefault) 
	{
		size_t stride = sizeof(TElement);
		return { GPUBufferType::kVertexBuffer, data, stride * count, stride, usage };
	}

	template<typename TElement>
	static GPUBufferDescription CreateIndex(
		const TElement* data, 
		size_t count, 
		GPUResourceUsage usage = GPUResourceUsage::kDefault) 
	{
		size_t stride = sizeof(TElement);
		return { GPUBufferType::kIndexBuffer, data, stride * count, stride, usage };
	}

	template<typename TElement>
	static GPUBufferDescription CreateConstant(
		const TElement* data, 
		size_t count, 
		GPUResourceUsage usage) 
	{
		size_t stride = sizeof(TElement);
		return { GPUBufferType::kConstantBuffer, data, stride * count, stride, usage };
	}

	template<typename TElement>
	static GPUBufferDescription CreateConstant(const TElement* data, GPUResourceUsage usage) {
		return CreateConstant<TElement>(data, 1, usage);
	}

	template<typename TElement>
	static GPUBufferDescription CreateConstant(GPUResourceUsage usage) {
		return CreateConstant<TElement>(nullptr, 1, usage);
	}
};

class GPUBuffer {
public:
	ID3D11Buffer* GetNative() const;
	const GPUBufferDescription& GetDescription() const;

	size_t GetElementsCount() const;
	size_t GetSize() const;
	size_t GetStride() const;

	bool IsVertexBuffer() const;
	bool IsIndexBuffer() const;
	bool IsConstantBuffer() const;
	bool IsDynamic() const;

	GPUBuffer(const GPUDevice& device, GPUBufferDescription description);

private:
	const GPUDevice& device_;
	GPUBufferDescription description_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;

	static D3D11_USAGE ToDXUsage(const GPUResourceUsage value);
	static D3D11_BIND_FLAG ToDXBufferType(const GPUBufferType buffer_type);
};

} // namespace aoe