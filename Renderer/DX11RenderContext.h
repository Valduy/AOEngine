#pragma once

#include "../Application/Window.h"
#include "../Graphics/DX11GPUSwapChain.h"

#include "DX11GBuffer.h"
#include "DX11RasterizerStates.h"
#include "DX11DepthStates.h"

namespace aoe {

class DX11RenderContext {
public:
	// TODO: handle window resize and update resources

	DX11RenderContext(const Window& window)
		: window_(window)
		, swap_chain_(window)
		, depth_buffer_(GPUTexture2DDescription::DepthStencilBuffer(window.GetWidth(), window.GetHeight()))
		, gbuffer_(window.GetWidth(), window.GetHeight())
	{}

	Viewport GetViewport() const {
		float width = static_cast<float>(window_.GetWidth());
		float height = static_cast<float>(window_.GetHeight());
		return { 0, 0, width, height, 0, 1 };
	}

	const DX11GPUTextureView& GetRenderTargetView() {
		return swap_chain_.GetRenderTargetView();
	}

	const DX11GPUTextureView& GetDepthBufferView() {
		return depth_buffer_.GetTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetDiffuseTextureView() {
		return gbuffer_.GetDiffuseTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetSpecularTextureView() {
		return gbuffer_.GetSpecularTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetNormalTextureView() {
		return gbuffer_.GetNormalTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetPositionTextureView() {
		return gbuffer_.GetPositionTexture()->GetTextureView();
	}

	const DX11GPUTextureView& GetAccumulatorTextureView() {
		return gbuffer_.GetAccumulatorTexture()->GetTextureView();
	}

	const DX11GPURasterizerState& GetRasterizerState(GPUCullMode cull_mode, GPUFillMode fill_mode) const {
		return rasterizer_states_.Get(cull_mode, fill_mode);
	}

	const DX11GPURasterizerState& GetRasterizerState(DX11RasterizerStateID id) const {
		return rasterizer_states_.Get(id);
	}

	const DX11GPUDepthState& GetDepthState(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function) const 
	{
		return depth_states_.Get(is_depth_enabled, write_mask, comparsion_function);
	}

	const DX11GPUDepthState& GetDepthState(DX11DepthStateID id) const {
		return depth_states_.Get(id);
	}

	const void PresentFrame() const {
		swap_chain_.Present();
	}

private:
	const Window& window_;

	DX11GPUSwapChain swap_chain_;
	DX11TextureScaler depth_buffer_;
	DX11GBuffer gbuffer_;
	DX11RasterizerStates rasterizer_states_;
	DX11DepthStates depth_states_;
};

} // namespace aoe