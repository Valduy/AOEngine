#include "pch.h"

#include "../Core/Debug.h"

#include "GPUSwapChain.h"
#include "DXHelper.h"

namespace aoe {

IDXGISwapChain* GPUSwapChain::GetNative() const {
	return swap_chain_.Get();
}

GPURenderTargetView GPUSwapChain::GetRenderTargetView() const {
	return { render_target_view_.Get()};
}

GPUSwapChain::GPUSwapChain(const GPUDevice& device, const Window& window)
	: device_(device)
	, window_(window)
	, dxgi_factory_(nullptr)
	, swap_chain_(nullptr)
	, back_buffer_(nullptr)
	, render_target_view_(nullptr)
	, width_(0)
	, height_(0)
{
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferDesc.Width = window_.GetWidth();
	swap_chain_desc.BufferDesc.Height = window_.GetHeight();
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

	hr = CreateDXGIFactory(IID_PPV_ARGS(dxgi_factory_.GetAddressOf()));
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create DXGI factory.");

	hr = dxgi_factory_->CreateSwapChain(device_.GetNative(), &swap_chain_desc, swap_chain_.GetAddressOf());
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create swap chain.");

	hr = swap_chain_->GetBuffer(0, IID_ID3D11Texture2D, reinterpret_cast<void**>(back_buffer_.GetAddressOf()));
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to get back buffer.");
	
	hr = device_.GetNative()->CreateRenderTargetView(back_buffer_.Get(), nullptr, &render_target_view_);
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create render target for back buffer.");
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