#pragma once

#include <cstdint>

#include "../Application/Window.h"
#include "../Game/Executor.h"

#include "GPUDevice.h"
#include "GPUTexture2D.h"

namespace aoe {

class GPUSwapChain : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUSwapChain)

	IDXGISwapChain* GetNative() const;
	GPURenderTargetView GetRenderTargetView() const;

	GPUSwapChain(const GPUDevice& device, const Window& window);

	bool Initialize();
	void Terminate() override;

	bool Resize(size_t width, size_t height);
	void Present();

private:
	IDXGIFactory* dxgi_factory_;
	IDXGISwapChain* swap_chain_;
	ID3D11Texture2D* back_buffer_;
	ID3D11RenderTargetView* render_target_view_;

	const GPUDevice& device_;
	const Window& window_;

	int32_t width_;
	int32_t height_;
};

} // namespace aoe