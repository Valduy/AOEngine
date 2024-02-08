#pragma once

#include "DX11GPUDevice.h"
#include "Viewport.h"
#include "IGPURasterizerState.h"
#include "IGPUDepthState.h"
#include "IGPUTexture2D.h"
#include "IGPUBuffer.h"
#include "DX11GPUVertexShader.h"
#include "DX11GPUPixelShader.h"
#include "IGPUSampler.h"

namespace aoe {

class DX11GPUContext {
public:
	ID3D11DeviceContext* GetNative() const;

	DX11GPUContext(ID3D11DeviceContext* context);

	void SetViewport(const Viewport& viewport);
	
	void SetRasterizerState(const IGPURasterizerState* rasterizer_state);
	
	void SetRenderTarget(const IGPUTexture2D& texture);
	void SetRenderTarget(const IGPUTextureView* render_target_view);
	void SetRenderTarget(
		const IGPUTextureView* render_target_view, 
		const IGPUTextureView* depth_stencil_view);
	
	void SetDepthState(const IGPUDepthState* depth_stencil_state);

	void SetVertexBuffer(const IGPUBuffer* buffer);
	void SetIndexBuffer(const IGPUBuffer* buffer);
	void SetConstantBuffer(const GPUShaderType shader_type, const IGPUBuffer* buffer, uint32_t slot = 0);

	template<typename TElement>
	void UpdateBuffer(const IGPUBuffer* buffer, const TElement* data, size_t count);
	void UpdateBuffer(const IGPUBuffer* buffer, const void* data, size_t size);

	void SetShaderResource(const GPUShaderType shader_type, const IGPUTexture2D* texture, uint32_t slot = 0);
	void SetShaderResource(const GPUShaderType shader_type, const IGPUTextureView* shader_resource_view, uint32_t slot = 0);
	
	void SetSampler(const GPUShaderType shader_type, const IGPUSampler* sampler, uint32_t slot = 0);

	void ClearState();
	void ClearRenderTarget(const IGPUTexture2D& texture, const float color[4]);
	void ClearRenderTarget(const IGPUTextureView* render_target_view, const float color[4]);
	
	void ClearDepth(const IGPUTexture2D& texture, float value);
	void ClearDepth(const IGPUTextureView* depth_stencil_view, float value);
	
	void SetPrimitiveTopology(const GPUPrimitiveTopology topology);
	
	void SetVertexShader(const DX11GPUVertexShader& vertex_shader);
	void SetPixelShader(const DX11GPUPixelShader& pixel_shader);
	
	void Draw(uint32_t vertex_count);
	void DrawIndexed(uint32_t index_count);
	
private:
	ID3D11DeviceContext* context_;

	static D3D11_VIEWPORT ToDXViewport(const Viewport& viewport);
	static D3D_PRIMITIVE_TOPOLOGY ToDXPrimitiveTopology(const GPUPrimitiveTopology value);
};

template<typename TElement>
void DX11GPUContext::UpdateBuffer(const IGPUBuffer* buffer, const TElement* data, size_t count) {
	const void* raw = data;
	size_t size = sizeof(TElement) * count;
	UpdateBuffer(buffer, raw, size);
}

} // namespace aoe