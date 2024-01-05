#pragma once

#include <vector>

#include "GPUDevice.h"

namespace aoe {

class GPUDevice;

template<typename TByte>
struct GPUBufferDescription {
	using Byte = TByte;

	static const size_t kByteWidth = sizeof(TByte);

	const TByte* data;
	size_t size;
	GPUResourceUsage usage;

	size_t GetMemorySize() const {
		return kByteWidth * size;
	}

	bool IsDynamic() const {
		return usage == GPUResourceUsage::kDynamic;
	}

	GPUBufferDescription()
		: GPUBufferDescription(nullptr, 0, GPUResourceUsage::kDefault)
	{}

	GPUBufferDescription(TByte* data, size_t size, GPUResourceUsage usage)
		: data(data)
		, size(size)
		, usage(usage)
	{}
};

template<typename TByte>
class GPUBuffer : public IGPUResource {
public:
	using Description = GPUBufferDescription<TByte>;

	AOE_NON_COPYABLE_CLASS(GPUBuffer<TByte>)

	ID3D11Buffer* GetNative() const {
		return buffer_;
	}

	const Description& GetDescription() const {
		return description_;
	}

	const size_t GetSize() const {
		return description_.size;
	}

	GPUBuffer(const GPUDevice& device)
		: device_(device)
		, description_()
		, buffer_(nullptr)
	{}

	bool Initialize(const Description& description) {
		AOE_ASSERT(buffer_ == nullptr);
		description_ = description;

		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.Usage = ToDXUsage(description_.usage);
		buffer_desc.BindFlags = GetBufferType();
		buffer_desc.CPUAccessFlags = description_.IsDynamic() ? D3D11_CPU_ACCESS_WRITE : 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		buffer_desc.ByteWidth = description_.GetMemorySize();

		D3D11_SUBRESOURCE_DATA buffer_data = {};
		buffer_data.pSysMem = description_.data;
		buffer_data.SysMemPitch = 0;
		buffer_data.SysMemSlicePitch = 0;

		HRESULT hr = device_.GetNative()->CreateBuffer(
			&buffer_desc, 
			description_.data == nullptr ? nullptr : &buffer_data, 
			&buffer_);
		return SUCCEEDED(hr);
	}

	void Terminate() override {
		AOE_DX_SAFE_RELEASE(buffer_);
	}

protected:
	virtual D3D11_BIND_FLAG GetBufferType() = 0;

private:
	const GPUDevice& device_;
	Description description_;
	ID3D11Buffer* buffer_;

	static D3D11_USAGE ToDXUsage(const GPUResourceUsage value) {
		return static_cast<D3D11_USAGE>(value);
	}
};

template<typename TByte>
class GPUVertexBuffer : public GPUBuffer<TByte> {
public:
	GPUVertexBuffer(const GPUDevice& device)
		: GPUBuffer<TByte>(device)
	{}

protected:
	D3D11_BIND_FLAG GetBufferType() override {
		return D3D11_BIND_VERTEX_BUFFER;
	}
};

class GPUIndexBuffer : public GPUBuffer<int32_t> {
public:
	GPUIndexBuffer(const GPUDevice& device)
		: GPUBuffer<int32_t>(device)
	{}

protected:
	D3D11_BIND_FLAG GetBufferType() override {
		return D3D11_BIND_INDEX_BUFFER;
	}
};

template<typename TByte>
class GPUConstantBuffer : public GPUBuffer<TByte> {
public:
	GPUConstantBuffer(const GPUDevice& device)
		: GPUBuffer<TByte>(device)
	{}

public:
	D3D11_BIND_FLAG GetBufferType() override {
		return D3D11_BIND_CONSTANT_BUFFER;
	}
};

} // namespace aoe