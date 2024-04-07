#include "pch.h"

#include "DX11GPUContext.h"

namespace aoe {

DX11GPUContext::DX11GPUContext(ID3D11DeviceContext* context)
	: context_(context)
{}

ID3D11DeviceContext* DX11GPUContext::GetNative() const {
	return context_;
}

void DX11GPUContext::SetViewport(const Viewport& viewport) {
	D3D11_VIEWPORT mapped = ToDXViewport(viewport);
	context_->RSSetViewports(1, &mapped);
}

void DX11GPUContext::SetRasterizerState(const DX11GPURasterizerState& rasterizer_state) {
	ID3D11RasterizerState* native = rasterizer_state.GetNative();
	context_->RSSetState(native);
}

void DX11GPUContext::SetRenderTargets(DX11GPURenderTargets render_targets) {
	const DX11GPUTextureView** render_target_views = render_targets.render_target_views;
	const DX11GPUTextureView* depth_stencil_view = render_targets.depth_stencil_view;
	const uint32_t count = DX11GPURenderTargets::kRenderTargetsCount;

	AOE_ASSERT_MSG(AreRenderTargetViewsValid(render_target_views, count), "Some of texture view is not render target view.");

	ID3D11RenderTargetView* native_render_target_views[DX11GPURenderTargets::kRenderTargetsCount];
	ID3D11DepthStencilView* native_depth_stencil_view = nullptr;

	for (size_t i = 0; i < count; ++i) {
		const DX11GPUTextureView* render_target_view = render_target_views[i];
		ID3D11RenderTargetView* native_render_target_view = nullptr;

		if (render_target_view != nullptr) {
			native_render_target_view = render_target_view->GetRenderTargetView();
		}

		native_render_target_views[i] = native_render_target_view;
	}

	if (depth_stencil_view != nullptr) {
		AOE_ASSERT_MSG(depth_stencil_view->IsDepthStencilView(), "Texture view is not depth stencil view.");
		native_depth_stencil_view = depth_stencil_view->GetDepthStencilView();
	}

	context_->OMSetRenderTargets(count, native_render_target_views, native_depth_stencil_view);
}

void DX11GPUContext::SetRenderTarget(const DX11GPUTextureView& render_target_view) {
	AOE_ASSERT_MSG(render_target_view.IsRenderTargetView(), "Texture view is not render target view.");

	ID3D11RenderTargetView* native_render_target_view = render_target_view.GetRenderTargetView();

	context_->OMSetRenderTargets(1, &native_render_target_view, nullptr);
}

void DX11GPUContext::SetRenderTarget(
	const DX11GPUTextureView& render_target_view, 
	const DX11GPUTextureView& depth_stencil_view) 
{
	AOE_ASSERT_MSG(render_target_view.IsRenderTargetView(), "Texture view is not render target view.");
	AOE_ASSERT_MSG(depth_stencil_view.IsDepthStencilView(), "Texture view is not depth stencil view.");

	ID3D11RenderTargetView* native_render_target_view = render_target_view.GetRenderTargetView();
	ID3D11DepthStencilView* native_depth_stencil_view = depth_stencil_view.GetDepthStencilView();

	context_->OMSetRenderTargets(1, &native_render_target_view, native_depth_stencil_view);
}

void DX11GPUContext::SetShaderResource(const GPUShaderType shader_type, const DX11GPUTextureView& shader_resource_view, uint32_t slot) {
	ID3D11ShaderResourceView* native = shader_resource_view.GetShaderResourceView();

	switch (shader_type) {
	case GPUShaderType::kVertex:
		context_->VSSetShaderResources(slot, 1, &native);
		return;
	case GPUShaderType::kPixel:
		context_->PSSetShaderResources(slot, 1, &native);
		return;
	}
}

void DX11GPUContext::SetDepthState(const DX11GPUDepthState& depth_stencil_state) {
	ID3D11DepthStencilState* native = depth_stencil_state.GetNative();
	context_->OMSetDepthStencilState(native, 0);
}

void DX11GPUContext::SetBlendState(const DX11GPUBlendState& blend_state, uint32_t sample_mask) {
	ID3D11BlendState* native = blend_state.GetNative();
	context_->OMSetBlendState(native, nullptr, sample_mask);
}

void DX11GPUContext::SetBlendState(const DX11GPUBlendState& blend_state, const float blend_factor[4], uint32_t sample_mask) {
	ID3D11BlendState* native = blend_state.GetNative();
	context_->OMSetBlendState(native, blend_factor, sample_mask);
}

void DX11GPUContext::SetVertexBuffer(const DX11GPUBuffer& buffer) {
	AOE_ASSERT_MSG(buffer.IsVertexBuffer(), "Buffer is not vertex buffer.");

	ID3D11Buffer* native = buffer.GetNative();

	const UINT strides[] = { buffer.GetStride()};
	const UINT offsets[] = { 0 };

	context_->IASetVertexBuffers(0, 1, &native, strides, offsets);
}

void DX11GPUContext::SetIndexBuffer(const DX11GPUBuffer& buffer) {
	AOE_ASSERT_MSG(buffer.IsIndexBuffer(), "Buffer is not index buffer.");

	ID3D11Buffer* native = buffer.GetNative();

	context_->IASetIndexBuffer(native, DXGI_FORMAT_R32_UINT, 0);
}

void DX11GPUContext::SetConstantBuffer(const GPUShaderType shader_type, const DX11GPUBuffer& buffer, uint32_t slot) {
	AOE_ASSERT_MSG(buffer.IsConstantBuffer(), "Buffer is not constant buffer.");

	ID3D11Buffer* native = buffer.GetNative();

	switch (shader_type) {
	case GPUShaderType::kVertex:
		context_->VSSetConstantBuffers(slot, 1, &native);
		return;
	case GPUShaderType::kPixel:
		context_->PSSetConstantBuffers(slot, 1, &native);
		return;
	}
}

void DX11GPUContext::UpdateBuffer(const DX11GPUBuffer& buffer, const void* data, uint32_t size) {
	AOE_ASSERT_MSG(data != nullptr, "Can't update buffer with null data.");
	AOE_ASSERT_MSG(buffer.GetSize() >= size, "Buffer size less than data size.");

	ID3D11Buffer* native = buffer.GetNative();

	if (buffer.IsDynamic()) {
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

void DX11GPUContext::SetSampler(const GPUShaderType shader_type, const DX11GPUSampler& sampler, uint32_t slot) {
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

void DX11GPUContext::ClearState() {
	context_->ClearState();
}

void DX11GPUContext::ClearRenderTarget(const DX11GPUTextureView& render_target_view, const float color[4]) {
	AOE_ASSERT_MSG(render_target_view.IsRenderTargetView(), "Texture view is not render target view.");

	ID3D11RenderTargetView* native = render_target_view.GetRenderTargetView();
	context_->ClearRenderTargetView(native, color);
}

void DX11GPUContext::ClearDepth(const DX11GPUTextureView& depth_stencil_view, float value) {
	AOE_ASSERT_MSG(depth_stencil_view.IsDepthStencilView(), "Texture view is not depth stencil view.");

	ID3D11DepthStencilView* native = depth_stencil_view.GetDepthStencilView();
	context_->ClearDepthStencilView(native, D3D11_CLEAR_DEPTH, value, 0);
}

void DX11GPUContext::SetPrimitiveTopology(const GPUPrimitiveTopology topology) {
	context_->IASetPrimitiveTopology(ToDXPrimitiveTopology(topology));
}

void DX11GPUContext::SetVertexShader(const DX11GPUVertexShader& vertex_shader) {
	ID3D11InputLayout* input_layout = vertex_shader.GetInputLayout();
	context_->IASetInputLayout(input_layout);
	context_->VSSetShader(vertex_shader.GetNative(), nullptr, 0);
}

void DX11GPUContext::SetPixelShader(const DX11GPUPixelShader& pixel_shader) {
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

bool DX11GPUContext::AreRenderTargetViewsValid(const DX11GPUTextureView** texture_views, size_t count) {
	for (size_t i = 0; i < count; ++i) {
		const DX11GPUTextureView* texture_view = texture_views[i];

		if (texture_view != nullptr && !texture_view->IsRenderTargetView()) {
			return false;
		}
	}

	return true;
}

} // namespace aoe