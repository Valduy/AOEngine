#include "pch.h"

#include "../Core/Debug.h"

#include "GPUSwapChain.h"
#include "DXHelper.h"

namespace aoe {

IDXGISwapChain* GPUSwapChain::GetNative() const {
	return swap_chain_;
}

GPURenderTargetView GPUSwapChain::GetRenderTargetView() const {
	return { render_target_view_ };
}

GPUSwapChain::GPUSwapChain(const GPUDevice& device, const Window& window)
	: dxgi_factory_(nullptr)
	, swap_chain_(nullptr)
	, back_buffer_(nullptr)
	, render_target_view_(nullptr)
	, device_(device)
	, window_(window)
	, width_(0)
	, height_(0)
{}

bool GPUSwapChain::Initialize() {
	AOE_ASSERT(dxgi_factory_ == nullptr);
	AOE_ASSERT(swap_chain_ == nullptr);
	AOE_ASSERT(back_buffer_ == nullptr);
	AOE_ASSERT(render_target_view_ == nullptr);

	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferDesc.Width = window_.GetWidth();
	swap_chain_desc.BufferDesc.Height = window_.GetHeight();
	//swap_chain_desc.BufferDesc.RefreshRate.Numerator = Executor::kFps;
	//swap_chain_desc.BufferDesc.RefreshRate.Denominator = 1;
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

	HRESULT hr = S_OK;
	hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgi_factory_));
	AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);

	hr = dxgi_factory_->CreateSwapChain(device_.GetNative(), &swap_chain_desc, &swap_chain_);
	AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);

	hr = swap_chain_->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(&back_buffer_));
	AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);

	hr = device_.GetNative()->CreateRenderTargetView(back_buffer_, nullptr, &render_target_view_);
	AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);

	return true;
}

void GPUSwapChain::Terminate() {
	AOE_DX_SAFE_RELEASE(render_target_view_);
	AOE_DX_SAFE_RELEASE(back_buffer_);
	AOE_DX_SAFE_RELEASE(swap_chain_);
	AOE_DX_SAFE_RELEASE(dxgi_factory_);
}

bool GPUSwapChain::Resize(size_t width, size_t height) {
	AOE_ASSERT(swap_chain_ != nullptr);

	if (width_ == width && height_ == height) {
		return false;
	}

	// TODO: implement resize logic
}

void GPUSwapChain::Present() {
	// TODO: vsync and present flags
	AOE_ASSERT(swap_chain_ != nullptr);
	swap_chain_->Present(1, 0);
}

} // namespace aoe