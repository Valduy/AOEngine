#pragma once

#include "GPUDevice.h"
#include "GPUPixelFormat.h"
#include "GPUResourceWrapper.h"
#include "IGPUTextureData.h"

namespace aoe {

using GPUShaderResourceView = GPUResourceWrapper<ID3D11ShaderResourceView>;
using GPUDepthStencilView = GPUResourceWrapper<ID3D11DepthStencilView>;
using GPURenderTargetView = GPUResourceWrapper<ID3D11RenderTargetView>;
using GPUUnorderedAccessView = GPUResourceWrapper<ID3D11UnorderedAccessView>;

struct GPUTexture2DDescription {
	uint32_t width;
	uint32_t height;
	GPUPixelFormat pixel_format;
	GPUTextureFlags texture_flags;
	const void* data;
	size_t size;
	size_t byte_width;
};

class GPUTexture2D : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUTexture2D)

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	GPUPixelFormat GetPixelFormat() const;

	bool IsShaderResource() const;
	bool IsDepthStencil() const;
	bool IsRenderTarget() const;
	bool IsUnorderedAccess() const;

	GPUShaderResourceView GetShaderResourceView() const;
	GPUDepthStencilView GetDepthStencilView() const;
	GPURenderTargetView GetRenderTargetView() const;
	GPUUnorderedAccessView GetUnorderedAccessView() const;

	GPUTexture2D(const GPUDevice& device);

	bool Initialize(
		uint32_t width, 
		uint32_t height, 
		GPUPixelFormat pixel_format, 
		GPUTextureFlags texture_flags, 
		const IGPUTextureData* texture_data = nullptr);
	void Terminate() override;

private:
	uint32_t width_;
	uint32_t height_;
	GPUPixelFormat pixel_format_;
	GPUTextureFlags texture_flags_;

	ID3D11Texture2D* texture_;
	ID3D11ShaderResourceView* shader_resource_view_;
	ID3D11DepthStencilView* depth_stencil_view_;
	ID3D11RenderTargetView* render_target_view_;
	ID3D11UnorderedAccessView* unordered_access_view_;

	const GPUDevice& device_;
};

} // namespace aoe