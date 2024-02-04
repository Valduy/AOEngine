#include "pch.h"

#include "../Core/Debug.h"

#include "DX11GPUDevice.h"
#include "DX11GPUContext.h"
#include "DX11GPUSwapChain.h"
#include "DX11GPUBuffer.h"
#include "DX11GPUTexture2D.h"
#include "DX11GPUDepthState.h"
#include "DX11GPURasterizerState.h"
#include "DX11GPUSampler.h"

namespace aoe {

ID3D11Device* DX11GPUDevice::GetNative() const {
	return device_.Get();
}

DX11GPUContext DX11GPUDevice::GetContext() const {
	AOE_ASSERT_MSG(context_ != nullptr, "GPUDevice is not initialized.");
	return { context_.Get()};
}

DX11GPUDevice::DX11GPUDevice()
	: device_(nullptr)
	, context_(nullptr)
{
	UINT device_flags =
		D3D11_CREATE_DEVICE_SINGLETHREADED |
		D3D11_CREATE_DEVICE_BGRA_SUPPORT |
		D3D11_CREATE_DEVICE_DEBUG; // TODO: wrap with ifndef preprocessor.

	const size_t feature_levels_count = 1;
	D3D_FEATURE_LEVEL feature_levels[feature_levels_count] = {
		D3D_FEATURE_LEVEL_11_1,
	};

	HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		device_flags,
		feature_levels,
		feature_levels_count,
		D3D11_SDK_VERSION,
		&device_,
		nullptr,
		&context_);

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create device and context.");
}

IGPUSwapChain* DX11GPUDevice::CreateSwapChain(const Window& window) const {
	return new DX11GPUSwapChain(*this, window);
}

IGPUBuffer* DX11GPUDevice::CreateBuffer(const GPUBufferDescription& description, const void* data, size_t size, size_t stride) const {
	return new DX11GPUBuffer(*this, description, data, size, stride);
}

IGPUTexture2D* DX11GPUDevice::CreateTexture2D(const GPUTexture2DDescription& description, const void* data, size_t stride) const {
	return new DX11GPUTexture2D(*this, description, data, stride);
}

IGPUDepthState* DX11GPUDevice::CreateDepthState(const GPUDepthStateDescription& description) const {
	return new DX11GPUDepthState(*this, description);
}

IGPURasterizerState* DX11GPUDevice::CreateRasterizerState(const GPURasterizerStateDescription& description) const {
	return new DX11GPURasterizerState(*this, description);
}

IGPUSampler* DX11GPUDevice::CreateSampler(const GPUSamplerDescription& description) const {
	return new DX11GPUSampler(*this, description);
}

} // namespace aoe