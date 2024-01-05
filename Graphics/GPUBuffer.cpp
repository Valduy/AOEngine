#include "pch.h"
#include "GPUBuffer.h"

namespace aoe {

ID3D11Buffer* GPUBuffer::GetNative() const {
	return buffer_;
}

const GPUBufferDescription& GPUBuffer::GetDescription() const {
	return description_;
}

size_t GPUBuffer::GetElementsCount() const {
	return description_.GetElementsCount();
}

size_t GPUBuffer::GetSize() const {
	return description_.size;
}

size_t GPUBuffer::GetStride() const {
	return description_.stride;
}

bool GPUBuffer::IsVertexBuffer() const {
	return description_.IsVertexBuffer();
}

bool GPUBuffer::IsIndexBuffer() const {
	return description_.IsIndexBuffer();
}

bool GPUBuffer::IsConstantBuffer() const {
	return description_.IsConstantBuffer();
}

bool GPUBuffer::IsDynamic() const {
	return description_.IsDynamic();
}

GPUBuffer::GPUBuffer(const GPUDevice& device)
	: device_(device)
	, description_()
	, buffer_(nullptr)
{}

bool GPUBuffer::Initialize(const GPUBufferDescription & description) {
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
	}
	else {
		hr = device_.GetNative()->CreateBuffer(&buffer_desc, nullptr, &buffer_);
	}

	return SUCCEEDED(hr);
}

void GPUBuffer::Terminate() {
	AOE_DX_SAFE_RELEASE(buffer_);
}

D3D11_USAGE GPUBuffer::ToDXUsage(const GPUResourceUsage value) {
	return static_cast<D3D11_USAGE>(value);
}

D3D11_BIND_FLAG GPUBuffer::ToDXBufferType(const GPUBufferType buffer_type) {
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

}