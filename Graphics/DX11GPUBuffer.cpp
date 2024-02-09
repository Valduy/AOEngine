#include "pch.h"

#include "../Core/Debug.h"

#include "DX11GPUBuffer.h"
#include "DX11Helper.h"

namespace aoe {

ID3D11Buffer* DX11GPUBuffer::GetNative() const {
	return buffer_.Get();
}

const GPUBufferDescription& DX11GPUBuffer::GetDescription() const {
	return description_;
}

uint32_t DX11GPUBuffer::GetElementsCount() const {
	return stride_ > 0 ? size_ / stride_ : 0;
}

uint32_t DX11GPUBuffer::GetSize() const {
	return size_;
}

uint32_t DX11GPUBuffer::GetStride() const {
	return stride_;
}

bool DX11GPUBuffer::IsVertexBuffer() const {
	return description_.IsVertexBuffer();
}

bool DX11GPUBuffer::IsIndexBuffer() const {
	return description_.IsIndexBuffer();
}

bool DX11GPUBuffer::IsConstantBuffer() const {
	return description_.IsConstantBuffer();
}

bool DX11GPUBuffer::IsDynamic() const {
	return description_.IsDynamic();
}

DX11GPUBuffer::DX11GPUBuffer(GPUBufferDescription description, const void* data, uint32_t size, uint32_t stride)
	: description_(std::move(description))
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

		hr = DX11GPUDevice::Instance()->GetNative()->CreateBuffer(&buffer_desc, &buffer_data, buffer_.GetAddressOf());
	}
	else {
		hr = DX11GPUDevice::Instance()->GetNative()->CreateBuffer(&buffer_desc, nullptr, buffer_.GetAddressOf());
	}

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create buffer.");
}

D3D11_USAGE DX11GPUBuffer::ToDXUsage(const GPUResourceUsage value) {
	return static_cast<D3D11_USAGE>(value);
}

D3D11_BIND_FLAG DX11GPUBuffer::ToDXBufferType(const GPUBufferType buffer_type) {
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