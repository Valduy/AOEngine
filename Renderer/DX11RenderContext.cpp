#include "pch.h"

#include "DX11RenderContext.h"

namespace aoe {

DX11RenderContext::DX11RenderContext(IWindow& window)
	: window_(window)
	, swap_chain_(window)
	, depth_buffer_(GPUTexture2DDescription::DepthStencilBuffer(window.GetWidth(), window.GetHeight()))
	, gbuffer_(window.GetWidth(), window.GetHeight())
{
	// TODO: handle window resize and update resources
}

Viewport DX11RenderContext::GetViewport() const {
	float width = static_cast<float>(window_.GetWidth());
	float height = static_cast<float>(window_.GetHeight());
	return { 0, 0, width, height, 0, 1 };
}

const DX11GPUTextureView& DX11RenderContext::GetRenderTargetView() {
	return swap_chain_.GetRenderTargetView();
}

const DX11GPUTextureView& DX11RenderContext::GetDepthBufferView() {
	return depth_buffer_.GetTexture()->GetTextureView();
}

const DX11GPUTextureView& DX11RenderContext::GetDiffuseTextureView() {
	return gbuffer_.GetDiffuseTexture()->GetTextureView();
}

const DX11GPUTextureView& DX11RenderContext::GetSpecularTextureView() {
	return gbuffer_.GetSpecularTexture()->GetTextureView();
}

const DX11GPUTextureView& DX11RenderContext::GetNormalTextureView() {
	return gbuffer_.GetNormalTexture()->GetTextureView();
}

const DX11GPUTextureView& DX11RenderContext::GetPositionTextureView() {
	return gbuffer_.GetPositionTexture()->GetTextureView();
}

const DX11GPUTextureView& DX11RenderContext::GetAccumulatorTextureView() {
	return gbuffer_.GetAccumulatorTexture()->GetTextureView();
}

const DX11GPURasterizerState& DX11RenderContext::GetRasterizerState(GPUCullMode cull_mode, GPUFillMode fill_mode) const {
	return rasterizer_states_.Get(cull_mode, fill_mode);
}

const DX11GPURasterizerState& DX11RenderContext::GetRasterizerState(DX11RasterizerStateID id) const {
	return rasterizer_states_.Get(id);
}

const DX11GPUDepthState& DX11RenderContext::GetDepthState(bool is_depth_enabled, GPUDepthWriteMask write_mask, GPUComparsionFunction comparsion_function) const
{
	return depth_states_.Get(is_depth_enabled, write_mask, comparsion_function);
}

const DX11GPUDepthState& DX11RenderContext::GetDepthState(DX11DepthStateID id) const {
	return depth_states_.Get(id);
}

const void DX11RenderContext::PresentFrame() const {
	swap_chain_.Present();
}

} // namespace aoe