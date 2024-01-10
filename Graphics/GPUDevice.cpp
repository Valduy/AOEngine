#include "pch.h"

#include "../Core/Debug.h"

#include "GPUDevice.h"
#include "GPUContext.h"

namespace aoe {

ID3D11Device* GPUDevice::GetNative() const {
	return device_.Get();
}

GPUContext GPUDevice::GetContext() const {
	AOE_ASSERT_MSG(context_ != nullptr, "GPUDevice is not initialized.");
	return { context_.Get()};
}

GPUDevice::GPUDevice()
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

} // namespace aoe