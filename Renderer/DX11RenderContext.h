#pragma once

#include "../Application/Window.h"
#include "../Graphics/DX11GPUSwapChain.h"

#include "DX11GBuffer.h"

namespace aoe {

class DX11RenderContext {
public:
	// TODO: handle window resize and update resources

	DX11RenderContext(const Window& window)
		: swap_chain_(window)
		, depth_buffer_(GPUTexture2DDescription::DepthStencilBuffer(window.GetWidth(), window.GetHeight()))
		, gbuffer_(window.GetWidth(), window.GetHeight())
	{}

	const DX11GPUTextureView& GetRenderTargetView() {
		return swap_chain_.GetRenderTargetView();
	}

	const DX11GPUTextureView& GetDepthBufferView() {
		return depth_buffer_.GetTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetDiffuseTextureView() {
		return gbuffer_.GetDiffuseTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetNormalTextureView() {
		return gbuffer_.GetNormalTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetPositionTextureView() {
		return gbuffer_.GetPositionTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetOutputTextureView() {
		return gbuffer_.GetOutputTexture()->GetTextureView();
	}

	const void PresentFrame() const {
		swap_chain_.Present();
	}

private:
	DX11GPUSwapChain swap_chain_;
	DX11TextureScaler depth_buffer_;
	DX11GBuffer gbuffer_;
};

} // namespace aoe