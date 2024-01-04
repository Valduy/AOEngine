#pragma once

#include <vector>

#include "GPUDevice.h"

namespace aoe {

class GPUDevice;

template<typename TByte>
struct GPUBufferDescription {
	using Byte = TByte;

	TByte* data;
	size_t size;
	GPUResourceUsage usage;

	GPUBufferDescription()
		: data(nullptr)
		, size(0)
		, usage(GPUResourceUsage::kDefault)
	{}
};

class GPUBuffer : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUBuffer)

	ID3D11Buffer* GetNative() const {
		return buffer_;
	}

	bool IsDynamic() const {
		return is_dynamic_;
	}

	virtual uint32_t GetByteWidth() const = 0;
	virtual uint32_t GetSize() const = 0;

	GPUBuffer(const GPUDevice& device)
		: buffer_(nullptr)
		, device_(device)
		, is_dynamic_(false)
	{}

	void Terminate() override {
		AOE_DX_SAFE_RELEASE(buffer_);
	}

protected:
	ID3D11Buffer* buffer_;

	const GPUDevice& device_;

	bool Initialize(
		D3D11_BIND_FLAG buffer_type, 
		uint32_t byte_width, 
		D3D11_SUBRESOURCE_DATA* data, 
		GPUResourceUsage usage = GPUResourceUsage::kDefault) 
	{
		AOE_ASSERT(buffer_ == nullptr);

		is_dynamic_ = usage == GPUResourceUsage::kDynamic;

		D3D11_BUFFER_DESC buffer_desc = {};
		buffer_desc.Usage = DXHelper::ToUsage(usage);
		buffer_desc.BindFlags = buffer_type;
		buffer_desc.CPUAccessFlags = is_dynamic_ ? D3D11_CPU_ACCESS_WRITE : 0;
		buffer_desc.MiscFlags = 0;
		buffer_desc.StructureByteStride = 0;
		buffer_desc.ByteWidth = byte_width;

		HRESULT hr = device_.GetNative()->CreateBuffer(&buffer_desc, data, &buffer_);
		return SUCCEEDED(hr);
	}

private:
	bool is_dynamic_;
};

template<typename TByte>
class GPUVertexBuffer : public GPUBuffer {
public:
	uint32_t GetByteWidth() const override {
		return sizeof(TByte);
	}

	uint32_t GetSize() const override {
		return vertices_.size();
	}

	GPUVertexBuffer(const GPUDevice& device)
		: GPUBuffer(device)
	{}

	bool Initialize(std::vector<TByte> vertices, GPUResourceUsage usage = GPUResourceUsage::kDefault) {
		vertices_ = std::move(vertices);

		D3D11_SUBRESOURCE_DATA vertex_data = {};
		vertex_data.pSysMem = vertices_.data();
		vertex_data.SysMemPitch = 0;
		vertex_data.SysMemSlicePitch = 0;

		uint32_t sizemem = GetByteWidth() * GetSize();
		return GPUBuffer::Initialize(D3D11_BIND_VERTEX_BUFFER, sizemem, &vertex_data, usage);
	}

private:
	std::vector<TByte> vertices_;
};

class GPUIndexBuffer : public GPUBuffer {
public:
	using Index = int32_t;

	uint32_t GetByteWidth() const override {
		return sizeof(Index);
	}

	uint32_t GetSize() const override {
		return indices_.size();
	}

	GPUIndexBuffer(const GPUDevice& device)
		: GPUBuffer(device)
	{}

	bool Initialize(std::vector<Index> indices, GPUResourceUsage usage = GPUResourceUsage::kDefault) {
		indices_ = std::move(indices);

		D3D11_SUBRESOURCE_DATA index_data = {};
		index_data.pSysMem = indices_.data();
		index_data.SysMemPitch = 0;
		index_data.SysMemSlicePitch = 0;

		uint32_t sizemem = GetByteWidth() * GetSize();
		return GPUBuffer::Initialize(D3D11_BIND_INDEX_BUFFER, sizemem, &index_data, usage);
	}

private:
	std::vector<Index> indices_;
};

template<typename TByte>
class GPUConstantBuffer : public GPUBuffer {
public:
	uint32_t GetByteWidth() const override {
		return sizeof(TByte);
	}

	uint32_t GetSize() const override {
		return 1;
	}

	GPUConstantBuffer(const GPUDevice& device)
		: GPUBuffer(device)
	{}

	bool Initialize(GPUResourceUsage usage = GPUResourceUsage::kDefault) {
		return GPUBuffer::Initialize(D3D11_BIND_CONSTANT_BUFFER, sizeof(TByte), nullptr, usage);
	}
};

} // namespace aoe