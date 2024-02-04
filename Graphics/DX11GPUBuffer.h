#pragma once

#include "IGPUBuffer.h"
#include "DX11GPUDevice.h"

namespace aoe {

class DX11GPUBuffer : public IGPUBuffer {
public:
	void* GetNative() const override;
	const GPUBufferDescription& GetDescription() const override;

	size_t GetElementsCount() const override;
	size_t GetSize() const override;
	size_t GetStride() const override;

	bool IsVertexBuffer() const override;
	bool IsIndexBuffer() const override;
	bool IsConstantBuffer() const override;
	bool IsDynamic() const override;

	DX11GPUBuffer(const DX11GPUDevice& device, GPUBufferDescription description, const void* data, size_t size, size_t stride);

private:
	const DX11GPUDevice& device_;
	GPUBufferDescription description_;
	Microsoft::WRL::ComPtr<ID3D11Buffer> buffer_;

	size_t size_;
	size_t stride_;

	static D3D11_USAGE ToDXUsage(const GPUResourceUsage value);
	static D3D11_BIND_FLAG ToDXBufferType(const GPUBufferType buffer_type);
};

} // namespace aoe