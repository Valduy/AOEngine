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

	template<typename TByte>
	void SetVertexBuffer(const GPUVertexBuffer<TByte>& buffer);
	void SetIndexBuffer(const GPUIndexBuffer& buffer);
	template<typename TByte>
	void SetConstantBuffer(const GPUShaderType shader_type, const GPUConstantBuffer<TByte>& buffer, uint32_t slot = 0);

	template<typename TByte>
	void UpdateBuffer(const GPUBuffer<TByte>& buffer, const TByte* data, size_t size);

	void PSSetShaderResource(const GPUTexture2D& texture, uint32_t slot);
	void PSSetShaderResource(const GPUShaderResourceView& shader_resource_view, uint32_t slot = 0);
	
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

template<typename TByte>
void GPUContext::SetVertexBuffer(const GPUVertexBuffer<TByte>& buffer) {
	ID3D11Buffer* native = buffer.GetNative();

	const UINT strides[] = { GPUBufferDescription<TByte>::kByteWidth };
	const UINT offsets[] = { 0 };

	context_->IASetVertexBuffers(0, 1, &native, strides, offsets);
}

template<typename TByte>
void GPUContext::SetConstantBuffer(const GPUShaderType shader_type, const GPUConstantBuffer<TByte>& buffer, uint32_t slot) {
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

template<typename TByte>
void GPUContext::UpdateBuffer(const GPUBuffer<TByte>& buffer, const TByte* data, size_t size) {
	AOE_ASSERT_MSG(data != nullptr, "Can't update buffer with null data.");
	AOE_ASSERT_MSG(buffer.GetSize() >= size, "Buffer size less than data size.");

	ID3D11Buffer* native = buffer.GetNative();
	const GPUBufferDescription<TByte>& description = buffer.GetDescription();
	const size_t memsize = sizeof(TByte) * size;

	if (description.IsDynamic()) {
		D3D11_MAPPED_SUBRESOURCE mapped_subresource;
		const HRESULT hr = context_->Map(native, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_subresource);
		AOE_DX_TRY_LOG_ERROR_AND_RETURN(hr);

		memcpy(mapped_subresource.pData, data, memsize);
		context_->Unmap(native, 0);
	}
	else {
		context_->UpdateSubresource(native, 0, nullptr, data, memsize, 0);
	}
}

} // namespace aoe