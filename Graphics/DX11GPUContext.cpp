#include "pch.h"

#include "DX11GPUContext.h"
#include "DX11GPUTextureView.h"

namespace aoe {

ID3D11DeviceContext* DX11GPUContext::GetNative() const {
	return context_;
}

DX11GPUContext::DX11GPUContext(ID3D11DeviceContext* context)
	: context_(context)
{}

void DX11GPUContext::SetViewport(const Viewport& viewport) {
	D3D11_VIEWPORT mapped = ToDXViewport(viewport);
	context_->RSSetViewports(1, &mapped);
}

void DX11GPUContext::SetRasterizerState(const IGPURasterizerState* rasterizer_state) {
	ID3D11RasterizerState* native = static_cast<ID3D11RasterizerState*>(rasterizer_state->GetNative());
	context_->RSSetState(native);
}

void DX11GPUContext::SetRenderTarget(const IGPUTexture2D& texture) {
	const IGPUTextureView* render_target_view = texture.GetTextureView();
	SetRenderTarget(render_target_view);
}

void DX11GPUContext::SetRenderTarget(const IGPUTextureView* render_target_view) {
	const DX11GPUTextureView* dx11_render_target_view = static_cast<const DX11GPUTextureView*>(render_target_view);

	AOE_ASSERT_MSG(dx11_render_target_view->IsRenderTargetView(), "Texture view is not render target view.");

	ID3D11RenderTargetView* native = dx11_render_target_view->GetRenderTargetView();
	context_->OMSetRenderTargets(1, &native, nullptr);
}

void DX11GPUContext::SetRenderTarget(
	const IGPUTextureView* render_target_view, 
	const IGPUTextureView* depth_stencil_view) 
{
	const DX11GPUTextureView* dx11_render_target_view = static_cast<const DX11GPUTextureView*>(render_target_view);
	const DX11GPUTextureView* dx11_depth_stencil_view = static_cast<const DX11GPUTextureView*>(depth_stencil_view);

	AOE_ASSERT_MSG(dx11_render_target_view->IsRenderTargetView(), "Texture view is not render target view.");
	AOE_ASSERT_MSG(dx11_depth_stencil_view->IsDepthStencilView(), "Texture view is not depth stencil view.");

	ID3D11RenderTargetView* native_render_target_view = dx11_render_target_view->GetRenderTargetView();
	ID3D11DepthStencilView* native_depth_stencil_view = dx11_depth_stencil_view->GetDepthStencilView();

	context_->OMSetRenderTargets(1, &native_render_target_view, native_depth_stencil_view);
}

void DX11GPUContext::SetDepthState(const IGPUDepthState* depth_stencil_state) {
	ID3D11DepthStencilState* native = static_cast<ID3D11DepthStencilState*>(depth_stencil_state->GetNative());
	context_->OMSetDepthStencilState(native, 0);
}

void DX11GPUContext::SetVertexBuffer(const IGPUBuffer* buffer) {
	AOE_ASSERT_MSG(buffer->IsVertexBuffer(), "Buffer is not vertex buffer.");

	ID3D11Buffer* native = static_cast<ID3D11Buffer*>(buffer->GetNative());

	const UINT strides[] = { buffer->GetStride()};
	const UINT offsets[] = { 0 };

	context_->IASetVertexBuffers(0, 1, &native, strides, offsets);
}

void DX11GPUContext::SetIndexBuffer(const IGPUBuffer* buffer) {
	AOE_ASSERT_MSG(buffer->IsIndexBuffer(), "Buffer is not index buffer.");

	ID3D11Buffer* native = static_cast<ID3D11Buffer*>(buffer->GetNative());

	context_->IASetIndexBuffer(native, DXGI_FORMAT_R32_UINT, 0);
}

void DX11GPUContext::SetConstantBuffer(const GPUShaderType shader_type, const IGPUBuffer* buffer, uint32_t slot) {
	AOE_ASSERT_MSG(buffer->IsConstantBuffer(), "Buffer is not constant buffer.");

	ID3D11Buffer* native = static_cast<ID3D11Buffer*>(buffer->GetNative());

	switch (shader_type) {
	case GPUShaderType::kVertex:
		context_->VSSetConstantBuffers(slot, 1, &native);
		return;
	case GPUShaderType::kPixel:
		context_->PSSetConstantBuffers(slot, 1, &native);
		return;
	}
}

void DX11GPUContext::UpdateBuffer(const IGPUBuffer* buffer, const void* data, size_t size) {
	AOE_ASSERT_MSG(data != nullptr, "Can't update buffer with null data.");
	AOE_ASSERT_MSG(buffer->GetSize() >= size, "Buffer size less than data size.");

	ID3D11Buffer* native = static_cast<ID3D11Buffer*>(buffer->GetNative());

	if (buffer->IsDynamic()) {
		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		const HRESULT hr = context_->Map(native, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		AOE_DX_TRY_LOG_ERROR_AND_RETURN(hr, "Failed to map subresource.");

		memcpy(mapped_subresource.pData, data, size);
		context_->Unmap(native, 0);
	}
	else {
		context_->UpdateSubresource(native, 0, nullptr, data, size, 0);
	}
}

void DX11GPUContext::SetShaderResource(const GPUShaderType shader_type, const IGPUTexture2D* texture, uint32_t slot) {
	const IGPUTextureView* shader_resource_view = texture->GetTextureView();
	SetShaderResource(shader_type, shader_resource_view, slot);
}

void DX11GPUContext::SetShaderResource(const GPUShaderType shader_type, const IGPUTextureView* shader_resource_view, uint32_t slot) {
	const DX11GPUTextureView* dx11_shader_resource_view = static_cast<const DX11GPUTextureView*>(shader_resource_view);
	ID3D11ShaderResourceView* native = dx11_shader_resource_view->GetShaderResourceView();

	switch (shader_type) {
	case GPUShaderType::kVertex:
		context_->VSSetShaderResources(slot, 1, &native);
		return;
	case GPUShaderType::kPixel:
		context_->PSSetShaderResources(slot, 1, &native);
		return;
	}
}

void DX11GPUContext::SetSampler(const GPUShaderType shader_type, const IGPUSampler* sampler, uint32_t slot) {
	ID3D11SamplerState* native = static_cast<ID3D11SamplerState*>(sampler->GetNative());

	switch (shader_type) {
	case GPUShaderType::kVertex:
		context_->VSSetSamplers(slot, 1, &native);
		return;
	case GPUShaderType::kPixel:
		context_->PSSetSamplers(slot, 1, &native);
		return;
	}
}

void DX11GPUContext::ClearState() {
	context_->ClearState();
}

void DX11GPUContext::ClearRenderTarget(const IGPUTexture2D& texture, const float color[4]) {
	const IGPUTextureView* render_target_view = texture.GetTextureView();
	ClearRenderTarget(render_target_view, color);
}

void DX11GPUContext::ClearRenderTarget(const IGPUTextureView* render_target_view, const float color[4]) {
	const DX11GPUTextureView* dx11_render_target_view = static_cast<const DX11GPUTextureView*>(render_target_view);

	AOE_ASSERT_MSG(dx11_render_target_view->IsRenderTargetView(), "Texture view is not render target view.");

	ID3D11RenderTargetView* native = dx11_render_target_view->GetRenderTargetView();
	context_->ClearRenderTargetView(native, color);
}

void DX11GPUContext::ClearDepth(const IGPUTexture2D& texture, float value) {
	const IGPUTextureView* depth_stencil_view = texture.GetTextureView();
	ClearDepth(depth_stencil_view, value);
}

void DX11GPUContext::ClearDepth(const IGPUTextureView* depth_stencil_view, float value) {
	const DX11GPUTextureView* dx11_depth_stencil_view = static_cast<const DX11GPUTextureView*>(depth_stencil_view);

	AOE_ASSERT_MSG(dx11_depth_stencil_view->IsDepthStencilView(), "Texture view is not depth stencil view.");

	ID3D11DepthStencilView* native = dx11_depth_stencil_view->GetDepthStencilView();
	context_->ClearDepthStencilView(native, D3D11_CLEAR_DEPTH, value, 0.0f);
}

void DX11GPUContext::SetPrimitiveTopology(const GPUPrimitiveTopology topology) {
	context_->IASetPrimitiveTopology(ToDXPrimitiveTopology(topology));
}

void DX11GPUContext::SetVertexShader(const GPUVertexShader& vertex_shader) {
	GPUInputLayout input_layout = vertex_shader.GetInputLayout();
	context_->IASetInputLayout(input_layout.GetNative());
	context_->VSSetShader(vertex_shader.GetNative(), nullptr, 0);
}

void DX11GPUContext::SetPixelShader(const GPUPixelShader& pixel_shader) {
	context_->PSSetShader(pixel_shader.GetNative(), nullptr, 0);
}

void DX11GPUContext::Draw(uint32_t vertex_count) {
	context_->Draw(vertex_count, 0);
}

void DX11GPUContext::DrawIndexed(uint32_t index_count) {
	context_->DrawIndexed(index_count, 0, 0);
}

D3D11_VIEWPORT DX11GPUContext::ToDXViewport(const Viewport& viewport) {
	return {
		viewport.x,
		viewport.y,
		viewport.width,
		viewport.height,
		viewport.min_depth,
		viewport.max_depth,
	};
}

inline D3D_PRIMITIVE_TOPOLOGY DX11GPUContext::ToDXPrimitiveTopology(const GPUPrimitiveTopology value) {
	return static_cast<D3D_PRIMITIVE_TOPOLOGY>(value);
}

} // namespace aoe