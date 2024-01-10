#pragma once

#include <cstdint>

#include "../Application/Window.h"

#include "GPUDevice.h"
#include "GPUTexture2D.h"

namespace aoe {

class GPUSwapChain {
public:
	IDXGISwapChain* GetNative() const;
	GPURenderTargetView GetRenderTargetView() const;

	GPUSwapChain(const GPUDevice& device, const Window& window);

	bool Resize(size_t width, size_t height);
	void Present();

private:
	const GPUDevice& device_;
	const Window& window_;

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgi_factory_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> back_buffer_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;

	int32_t width_;
	int32_t height_;
};

} // namespace aoe