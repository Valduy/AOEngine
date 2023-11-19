#pragma once

#include <cstdint>

#include "../Application/Window.h"
#include "../Game/Executor.h"

#include "GPUDevice.h"

namespace aoe {

class GPUSwapChain {
public:
	GPUSwapChain(const GPUDevice& device, const Window& window) 
		: dxgi_factory_(nullptr)
		, swap_chain_(nullptr)
		, device_(device)
		, window_(window)
		, width_(0)
		, height_(0)
	{}

	bool Initialize() {
		DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
		swap_chain_desc.BufferDesc.Width = window_.GetWidth();
		swap_chain_desc.BufferDesc.Height = window_.GetHeight();
		swap_chain_desc.BufferDesc.RefreshRate.Numerator = Executor::kFps;
		swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
		swap_chain_desc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swap_chain_desc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		swap_chain_desc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swap_chain_desc.SampleDesc.Count = 1;
		swap_chain_desc.SampleDesc.Quality = 0;
		swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swap_chain_desc.BufferCount = 2;
		swap_chain_desc.OutputWindow = window_.GetHandler();
		swap_chain_desc.Windowed = true;
		swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		swap_chain_desc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		if (FAILED(CreateDXGIFactory1(IID_PPV_ARGS(&dxgi_factory_)))) {
			return false;
		}

		HRESULT hr = dxgi_factory_->CreateSwapChain(
			device_.GetNative(),
			&swap_chain_desc,
			&swap_chain_);

		return SUCCEEDED(hr);
	}

	void Terminate() {
		AOE_ASSERT(dxgi_factory_ != nullptr);
		AOE_ASSERT(swap_chain_ != nullptr);

		dxgi_factory_->Release();
		dxgi_factory_ = nullptr;

		swap_chain_->Release();
		swap_chain_ = nullptr;
	}

	bool Resize(size_t width, size_t height) {
		AOE_ASSERT(swap_chain_ != nullptr);

		if (width_ == width && height_ == height) {
			return false;
		}

		// TODO:
	}

private:
	IDXGIFactory1* dxgi_factory_;
	IDXGISwapChain* swap_chain_;
	const GPUDevice& device_;
	const Window& window_;

	int32_t width_;
	int32_t height_;
};

} // namespace aoe