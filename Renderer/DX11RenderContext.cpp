#include "pch.h"

#include "DX11RenderContext.h"

namespace aoe {

DX11RenderContext::DX11RenderContext(IWindow& window)
	: window_(window)
	, swap_chain_(static_cast<HWND>(window.GetNative()), window.GetWidth(), window.GetHeight())
	, depth_buffer_(GPUTexture2DDescription::DepthStencilBuffer(window.GetWidth(), window.GetHeight()))
	, gbuffer_(window.GetWidth(), window.GetHeight())
{}

DX11RenderContext::~DX11RenderContext() {}

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

void DX11RenderContext::UpdateResources() {
	uint32_t resource_width = static_cast<uint32_t>(window_.GetWidth());
	uint32_t resource_height = static_cast<uint32_t>(window_.GetHeight());

	swap_chain_.TryResize(resource_width, resource_height);
	depth_buffer_.TryResize(resource_width, resource_height);
	gbuffer_.TryResize(resource_width, resource_height);
}

void DX11RenderContext::PresentFrame() const {
	swap_chain_.Present();
}

//void DX11RenderContext::OnWindowSizeChanged(int32_t width, int32_t height) {
//	uint32_t resource_width = static_cast<uint32_t>(width);
//	uint32_t resource_height = static_cast<uint32_t>(height);
//
//	swap_chain_.TryResize(resource_width, resource_height);
//	depth_buffer_.TryResize(resource_width, resource_height);
//	gbuffer_.TryResize(resource_width, resource_height);
//}

} // namespace aoe