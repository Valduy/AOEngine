#pragma once

#include "GPUDevice.h"
#include "Viewport.h"
#include "GPURasterizerState.h"
#include "GPUDepthState.h"
#include "GPUTexture2D.h"
#include "GPUBuffers.h"
#include "GPUShader.h"
#include "GPUSampler.h"

namespace aoe {

class GPUContext {
public:
	ID3D11DeviceContext* GetNative() const;

	GPUContext(ID3D11DeviceContext* context);

	void SetViewport(const Viewport& viewport);
	
	void SetRasterizerState(const GPURasterizerState& rasterizer_state);
	
	void SetRenderTarget(const GPUTexture2D& texture);
	void SetRenderTarget(const GPURenderTargetView& render_target_view);
	void SetRenderTarget(
		const GPURenderTargetView& render_target_view, 
		const GPUDepthStencilView& depth_stencil_view);
	
	void SetDepthState(const GPUDepthState& depth_stencil_state);

	void SetVertexBuffer(const GPUBuffer& buffer);
	void SetIndexBuffer(const GPUBuffer& buffer);
	void SetConstantBuffer(const GPUShaderType shader_type, const GPUBuffer& buffer, uint32_t slot = 0);

	template<typename TElement>
	void UpdateBuffer(const GPUBuffer& buffer, const TElement* data, size_t count);
	void UpdateBuffer(const GPUBuffer& buffer, const void* data, size_t size);

	void SetShaderResource(const GPUShaderType shader_type, const GPUTexture2D& texture, uint32_t slot);
	void SetShaderResource(const GPUShaderType shader_type, const GPUShaderResourceView& shader_resource_view, uint32_t slot = 0);
	
	void SetSampler(const GPUShaderType shader_type, const GPUSampler& sampler, uint32_t slot = 0);

	void ClearState();
	void ClearRenderTarget(const GPUTexture2D& texture, const float color[4]);
	void ClearRenderTarget(const GPURenderTargetView& render_target_view, const float color[4]);
	
	void ClearDepth(const GPUTexture2D& texture, float value);
	void ClearDepth(const GPUDepthStencilView& depth_stencil_view, float value);
	
	void SetPrimitiveTopology(const GPUPrimitiveTopology topology);
	
	void SetVertexShader(const GPUVertexShader& vertex_shader);
	void SetPixelShader(const GPUPixelShader& pixel_shader);
	
	void Draw(uint32_t vertex_count);
	void DrawIndexed(uint32_t index_count);
	
private:
	ID3D11DeviceContext* context_;

	static D3D11_VIEWPORT ToDXViewport(const Viewport& viewport);
	static D3D_PRIMITIVE_TOPOLOGY ToDXPrimitiveTopology(const GPUPrimitiveTopology value);
};

template<typename TElement>
void GPUContext::UpdateBuffer(const GPUBuffer& buffer, const TElement* data, size_t count) {
	const void* raw = data;
	size_t size = sizeof(TElement) * count;
	UpdateBuffer(buffer, raw, size);
}

} // namespace aoe