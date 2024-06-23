#pragma once

#include "../Graphics/DX11GPUSwapChain.h"
#include "../Graphics/Viewport.h"

#include "DX11GBuffer.h"
#include "DX11RasterizerStates.h"
#include "DX11DepthStates.h"

namespace aoe {

class DX11RenderContext {
public:
	DX11RenderContext(IWindow& window);
	
	Viewport GetViewport() const;

	const DX11GPUTextureView& GetRenderTargetView();
	const DX11GPUTextureView& GetDepthBufferView();
	const DX11GPUTextureView& GetDiffuseTextureView();
	const DX11GPUTextureView& GetSpecularTextureView();
	const DX11GPUTextureView& GetNormalTextureView();
	const DX11GPUTextureView& GetPositionTextureView();
	const DX11GPUTextureView& GetAccumulatorTextureView();

	const DX11GPURasterizerState& GetRasterizerState(GPUCullMode cull_mode, GPUFillMode fill_mode) const;
	const DX11GPURasterizerState& GetRasterizerState(DX11RasterizerStateID id) const;

	const DX11GPUDepthState& GetDepthState(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function) const;

	const DX11GPUDepthState& GetDepthState(DX11DepthStateID id) const;

	void UpdateResources();
	void PresentFrame() const;

private:
	IWindow& window_;

	DX11GPUSwapChain swap_chain_;
	DX11TextureScaler depth_buffer_;
	DX11GBuffer gbuffer_;
	DX11RasterizerStates rasterizer_states_;
	DX11DepthStates depth_states_;
};

} // namespace aoe