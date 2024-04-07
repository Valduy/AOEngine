#pragma once

#include <cstdint>

#include "../Application/Window.h"

#include "DX11GPUDevice.h"
#include "DX11GPUTexture2D.h"

namespace aoe {

class DX11GPUSwapChain {
public:
	DX11GPUSwapChain(const Window& window);
	~DX11GPUSwapChain();

	IDXGISwapChain* GetNative() const;
	const DX11GPUTextureView& GetRenderTargetView() const;

	bool Resize(uint32_t width, uint32_t height) const;
	void Present() const;

private:
	const Window& window_;

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgi_factory_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
	
	DX11GPUTexture2D* back_buffer_;

	int32_t width_;
	int32_t height_;
};

} // namespace aoe