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

	bool IsVertexBuffer() const {
		return buffer_type == GPUBufferType::kVertexBuffer;
	}

	bool IsIndexBuffer() const {
		return buffer_type == GPUBufferType::kIndexBuffer;
	}

	bool IsConstantBuffer() const {
		return buffer_type == GPUBufferType::kConstantBuffer;
	}

	size_t GetElementsCount() const {
		return stride > 0 ? size / stride : 0;
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

class GPUBuffer : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUBuffer)

	ID3D11Buffer* GetNative() const {
		return buffer_;
	}

	const GPUBufferDescription& GetDescription() const {
		return description_;
	}

	GPUBuffer(const GPUDevice& device)
		: device_(device)
		, description_()
		, buffer_(nullptr)
	{}

	bool Initialize(const GPUBufferDescription& description) {
		AOE_ASSERT(buffer_ == nullptr);
		description_ = description;

		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.Usage = ToDXUsage(description_.usage);
		buffer_desc.BindFlags = ToDXBufferType(description_.buffer_type);
		buffer_desc.CPUAccessFlags = description_.IsDynamic() ? D3D11_CPU_ACCESS_WRITE : 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		buffer_desc.ByteWidth = description_.size;
			
		HRESULT hr = S_OK;

		if (description_.data != nullptr) {
			D3D11_SUBRESOURCE_DATA buffer_data = {};
			buffer_data.pSysMem = description_.data;
			buffer_data.SysMemPitch = 0;
			buffer_data.SysMemSlicePitch = 0;

			hr = device_.GetNative()->CreateBuffer(&buffer_desc, &buffer_data, &buffer_);
		} else {
			hr = device_.GetNative()->CreateBuffer(&buffer_desc, nullptr, &buffer_);
		}

		return SUCCEEDED(hr);
	}

	void Terminate() override {
		AOE_DX_SAFE_RELEASE(buffer_);
	}

private:
	const GPUDevice& device_;
	GPUBufferDescription description_;
	ID3D11Buffer* buffer_;

	static D3D11_USAGE ToDXUsage(const GPUResourceUsage value) {
		return static_cast<D3D11_USAGE>(value);
	}

	static D3D11_BIND_FLAG ToDXBufferType(const GPUBufferType buffer_type) {
		switch (buffer_type) {
		case GPUBufferType::kVertexBuffer:
			return D3D11_BIND_VERTEX_BUFFER;
		case GPUBufferType::kIndexBuffer:
			return D3D11_BIND_INDEX_BUFFER;
		case GPUBufferType::kConstantBuffer:
			return D3D11_BIND_CONSTANT_BUFFER;
		default:
			AOE_ASSERT_MSG(false, "Unexpected buffer type.");
			return D3D11_BIND_VERTEX_BUFFER;
		}
	}
};

} // namespace aoe