#pragma once

#include "../Core/Debug.h"

#include "DXHeaders.h"
#include "GPUContext.h"

namespace aoe {

class GPUDevice {
public:
	ID3D11Device* GetNative() const {
		return device_;
	}

	GPUDevice()
		: device_(nullptr)
		, context_(nullptr)
	{}

	bool Initialize() {
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
		
		return SUCCEEDED(hr);
	}

	void Terminate() {
		AOE_ASSERT(device_ != nullptr);
		AOE_ASSERT(context_ != nullptr)

		device_->Release();
		device_ = nullptr;

		context_->Release();
		context_ = nullptr;
	}

	GPUContext CreateContext() const {
		AOE_ASSERT(context_ != nullptr);
		return { context_ };
	}

private:
	ID3D11Device* device_;
	ID3D11DeviceContext* context_;
};

} // namespace aoe