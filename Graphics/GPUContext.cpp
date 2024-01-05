#include "pch.h"

#include "GPUContext.h"

namespace aoe {

ID3D11DeviceContext* GPUContext::GetNative() const {
	return context_;
}

GPUContext::GPUContext(ID3D11DeviceContext* context)
	: context_(context)
{}

void GPUContext::SetViewport(const Viewport& viewport) {
	D3D11_VIEWPORT mapped = ToDXViewport(viewport);
	context_->RSSetViewports(1, &mapped);
}

void GPUContext::SetRasterizerState(const GPURasterizerState& rasterizer_state) {
	ID3D11RasterizerState* native = rasterizer_state.GetNative();
	context_->RSSetState(native);
}

void GPUContext::SetRenderTarget(const GPUTexture2D& texture) {
	GPURenderTargetView render_target_view = texture.GetRenderTargetView();
	SetRenderTarget(render_target_view);
}

void GPUContext::SetRenderTarget(const GPURenderTargetView& render_target_view) {
	ID3D11RenderTargetView* native = render_target_view.GetNative();
	context_->OMSetRenderTargets(1, &native, nullptr);
}

void GPUContext::SetRenderTarget(
	const GPURenderTargetView& render_target_view, 
	const GPUDepthStencilView& depth_stencil_view) 
{
	ID3D11RenderTargetView* native_render_target_view = render_target_view.GetNative();
	ID3D11DepthStencilView* native_depth_stencil_view = depth_stencil_view.GetNative();
	context_->OMSetRenderTargets(1, &native_render_target_view, native_depth_stencil_view);
}

void GPUContext::SetDepthState(const GPUDepthState& depth_stencil_state) {
	ID3D11DepthStencilState* native = depth_stencil_state.GetNative();
	context_->OMSetDepthStencilState(native, 0);
}

void GPUContext::SetIndexBuffer(const GPUIndexBuffer& buffer) {
	ID3D11Buffer* native = buffer.GetNative();
	context_->IASetIndexBuffer(native, DXGI_FORMAT_R32_UINT, 0);
}

void GPUContext::PSSetShaderResource(const GPUTexture2D& texture, uint32_t slot) {
	GPUShaderResourceView shader_resource_view = texture.GetShaderResourceView();
	PSSetShaderResource(shader_resource_view, slot);
}

void GPUContext::PSSetShaderResource(const GPUShaderResourceView& shader_resource_view, uint32_t slot) {
	ID3D11ShaderResourceView* native = shader_resource_view.GetNative();
	context_->PSSetShaderResources(slot, 1, &native);
}

void GPUContext::SetSampler(const GPUShaderType shader_type, const GPUSampler& sampler, uint32_t slot) {
	ID3D11SamplerState* native = sampler.GetNative();

	switch (shader_type) {
	case GPUShaderType::kVertex:
		context_->VSSetSamplers(slot, 1, &native);
		return;
	case GPUShaderType::kPixel:
		context_->PSSetSamplers(slot, 1, &native);
		return;
	}
}

void GPUContext::ClearState() {
	context_->ClearState();
}

void GPUContext::ClearRenderTarget(const GPUTexture2D& texture, const float color[4]) {
	GPURenderTargetView render_target_view = texture.GetRenderTargetView();
	ClearRenderTarget(render_target_view, color);
}

void GPUContext::ClearRenderTarget(const GPURenderTargetView& render_target_view, const float color[4]) {
	ID3D11RenderTargetView* native = render_target_view.GetNative();
	context_->ClearRenderTargetView(native, color);
}

void GPUContext::ClearDepth(const GPUTexture2D& texture, float value) {
	GPUDepthStencilView depth_stencil_view = texture.GetDepthStencilView();
	ClearDepth(depth_stencil_view, value);
}

void GPUContext::ClearDepth(const GPUDepthStencilView& depth_stencil_view, float value) {
	ID3D11DepthStencilView* native = depth_stencil_view.GetNative();
	context_->ClearDepthStencilView(native, D3D11_CLEAR_DEPTH, value, 0.0f);
}

void GPUContext::SetPrimitiveTopology(const GPUPrimitiveTopology topology) {
	context_->IASetPrimitiveTopology(ToDXPrimitiveTopology(topology));
}

void GPUContext::SetVertexShader(const GPUVertexShader& vertex_shader) {
	GPUInputLayout input_layout = vertex_shader.GetInputLayout();
	context_->IASetInputLayout(input_layout.GetNative());
	context_->VSSetShader(vertex_shader.GetNative(), nullptr, 0);
}

void GPUContext::SetPixelShader(const GPUPixelShader& pixel_shader) {
	context_->PSSetShader(pixel_shader.GetNative(), nullptr, 0);
}

void GPUContext::Draw(uint32_t vertex_count) {
	context_->Draw(vertex_count, 0);
}

void GPUContext::DrawIndexed(uint32_t index_count) {
	context_->DrawIndexed(index_count, 0, 0);
}

D3D11_VIEWPORT GPUContext::ToDXViewport(const Viewport& viewport) {
	return {
		viewport.x,
		viewport.y,
		viewport.width,
		viewport.height,
		viewport.min_depth,
		viewport.max_depth,
	};
}

inline D3D_PRIMITIVE_TOPOLOGY GPUContext::ToDXPrimitiveTopology(const GPUPrimitiveTopology value) {
	return static_cast<D3D_PRIMITIVE_TOPOLOGY>(value);
}

} // namespace aoe