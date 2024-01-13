#include "pch.h"
#include "GPUBuffer.h"

namespace aoe {

ID3D11Buffer* GPUBuffer::GetNative() const {
	return buffer_.Get();
}

const GPUBufferDescription& GPUBuffer::GetDescription() const {
	return description_;
}

size_t GPUBuffer::GetElementsCount() const {
	return stride_ > 0 ? size_ / stride_ : 0;
}

size_t GPUBuffer::GetSize() const {
	return size_;
}

size_t GPUBuffer::GetStride() const {
	return stride_;
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

GPUBuffer::GPUBuffer(const GPUDevice& device, GPUBufferDescription description, const void* data, size_t size, size_t stride)
	: device_(device)
	, description_(std::move(description))
	, buffer_(nullptr)
	, size_(size)
	, stride_(stride)
{
	D3D11_BUFFER_DESC buffer_desc = {};
	buffer_desc.Usage = ToDXUsage(description_.usage);
	buffer_desc.BindFlags = ToDXBufferType(description_.buffer_type);
	buffer_desc.CPUAccessFlags = description_.IsDynamic() ? D3D11_CPU_ACCESS_WRITE : 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	buffer_desc.ByteWidth = size;

	HRESULT hr = S_OK;

	if (data != nullptr) {
		D3D11_SUBRESOURCE_DATA buffer_data = {};
		buffer_data.pSysMem = data;
		buffer_data.SysMemPitch = 0;
		buffer_data.SysMemSlicePitch = 0;

		hr = device_.GetNative()->CreateBuffer(&buffer_desc, &buffer_data, buffer_.GetAddressOf());
	}
	else {
		hr = device_.GetNative()->CreateBuffer(&buffer_desc, nullptr, buffer_.GetAddressOf());
	}

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create buffer.");
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