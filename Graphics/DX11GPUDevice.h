#pragma once

#include <wrl/client.h>

#include "IGPUDevice.h"
#include "DX11Headers.h"

namespace aoe {

class DX11GPUContext;

class DX11GPUDevice : public IGPUDevice {
public:
	ID3D11Device* GetNative() const;
	DX11GPUContext GetContext() const;

	DX11GPUDevice();

	IGPUSwapChain* CreateSwapChain(const Window& window) const;
	IGPUBuffer* CreateBuffer(const GPUBufferDescription& description, const void* data, size_t size, size_t stride) const override;
	IGPUTexture2D* CreateTexture2D(const GPUTexture2DDescription& description, const void* data = nullptr, size_t stride = 0) const override;
	IGPUDepthState* CreateDepthState(const GPUDepthStateDescription& description) const override;
	IGPURasterizerState* CreateRasterizerState(const GPURasterizerStateDescription& description) const override;
	IGPUSampler* CreateSampler(const GPUSamplerDescription& description) const override;

private:
	Microsoft::WRL::ComPtr<ID3D11Device> device_;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> context_;
};

} // namespace aoe