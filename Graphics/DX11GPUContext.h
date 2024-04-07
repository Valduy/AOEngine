#pragma once

#include "DX11Helper.h"
#include "DX11GPUDevice.h"
#include "DX11GPURasterizerState.h"
#include "DX11GPUDepthState.h"
#include "DX11GPUBlendState.h"
#include "DX11GPUTexture2D.h"
#include "DX11GPURenderTargets.h"
#include "DX11GPUBuffer.h"
#include "DX11GPUVertexShader.h"
#include "DX11GPUPixelShader.h"
#include "DX11GPUSampler.h"
#include "Viewport.h"

namespace aoe {

class DX11GPUContext {
public:
	DX11GPUContext(ID3D11DeviceContext* context);

	ID3D11DeviceContext* GetNative() const;

	void SetViewport(const Viewport& viewport);
	
	void SetRasterizerState(const DX11GPURasterizerState& rasterizer_state);
	
	void SetRenderTargets(DX11GPURenderTargets render_targets);
	void SetRenderTarget(const DX11GPUTextureView& render_target_view);
	void SetRenderTarget(
		const DX11GPUTextureView& render_target_view, 
		const DX11GPUTextureView& depth_stencil_view);
	
	void SetShaderResource(const GPUShaderType shader_type, const DX11GPUTextureView& shader_resource_view, uint32_t slot = 0);

	void SetDepthState(const DX11GPUDepthState& depth_stencil_state);
	void SetBlendState(const DX11GPUBlendState& blend_state, uint32_t sample_mask);
	void SetBlendState(const DX11GPUBlendState& blend_state, const float blend_factor[4], uint32_t sample_mask);

	void SetVertexBuffer(const DX11GPUBuffer& buffer);
	void SetIndexBuffer(const DX11GPUBuffer& buffer);
	void SetConstantBuffer(const GPUShaderType shader_type, const DX11GPUBuffer& buffer, uint32_t slot = 0);

	template<typename TElement>
	void UpdateBuffer(const DX11GPUBuffer& buffer, const TElement* data, size_t count);
	void UpdateBuffer(const DX11GPUBuffer& buffer, const void* data, uint32_t size);

	void SetSampler(const GPUShaderType shader_type, const DX11GPUSampler& sampler, uint32_t slot = 0);

	void ClearState();
	void ClearRenderTarget(const DX11GPUTextureView& render_target_view, const float color[4]);
	void ClearDepth(const DX11GPUTextureView& depth_stencil_view, float value);
	
	void SetPrimitiveTopology(const GPUPrimitiveTopology topology);
	
	void SetVertexShader(const DX11GPUVertexShader& vertex_shader);
	void SetPixelShader(const DX11GPUPixelShader& pixel_shader);
	
	void Draw(uint32_t vertex_count);
	void DrawIndexed(uint32_t index_count);
	
private:
	ID3D11DeviceContext* context_;

	static D3D11_VIEWPORT ToDXViewport(const Viewport& viewport);
	static D3D_PRIMITIVE_TOPOLOGY ToDXPrimitiveTopology(const GPUPrimitiveTopology value);

	static bool AreRenderTargetViewsValid(const DX11GPUTextureView** texture_views, size_t count);
};

template<typename TElement>
void DX11GPUContext::UpdateBuffer(const DX11GPUBuffer& buffer, const TElement* data, size_t count) {
	const void* raw = data;
	uint32_t stride = sizeof(TElement);
	uint32_t size = stride * static_cast<uint32_t>(count);
	UpdateBuffer(buffer, raw, size);
}

} // namespace aoe