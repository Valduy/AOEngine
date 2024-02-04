#pragma once

#include <cstdint>

#include "IGPUSwapChain.h"
#include "DX11GPUDevice.h"
#include "DX11GPUTexture2D.h"

namespace aoe {

class DX11GPUSwapChain : public IGPUSwapChain {
public:
	void* GetNative() const override;
	const IGPUTextureView* GetRenderTargetView() const override;

	DX11GPUSwapChain(const DX11GPUDevice& device, const Window& window);
	~DX11GPUSwapChain() override;

	bool Resize(size_t width, size_t height) override;
	void Present() override;

private:
	const DX11GPUDevice& device_;
	const Window& window_;

	Microsoft::WRL::ComPtr<IDXGIFactory> dxgi_factory_;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swap_chain_;
	
	DX11GPUTexture2D* back_buffer_;

	int32_t width_;
	int32_t height_;
};

} // namespace aoe