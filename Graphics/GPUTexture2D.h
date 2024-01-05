#pragma once

#include "GPUDevice.h"
#include "GPUPixelFormat.h"
#include "GPUResourceWrapper.h"

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
	size_t stride;

	GPUTexture2DDescription()
		: width(0)
		, height(0)
		, pixel_format(GPUPixelFormat::kUnknown)
		, texture_flags(GPUTextureFlags::kNone)
		, data(nullptr)
		, stride(0)
	{}

	template<typename TElement>
	static GPUTexture2DDescription Create(
		uint32_t width, 
		uint32_t height, 
		GPUPixelFormat pixel_format,
		GPUTextureFlags texture_flags,
		const TElement* data)
	{
		GPUTexture2DDescription result;
		result.width = width;
		result.height = height;
		result.pixel_format = pixel_format;
		result.texture_flags = texture_flags;
		result.data = data;
		result.stride = sizeof(uint32_t);
		return result;
	}
};

class GPUTexture2D : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUTexture2D)

	const ID3D11Texture2D* GetNative() const;
	const GPUTexture2DDescription& GetDescription() const;

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

	bool Initialize(const GPUTexture2DDescription& description);
	void Terminate() override;

private:
	const GPUDevice& device_;
	GPUTexture2DDescription description_;
	ID3D11Texture2D* texture_;
	ID3D11ShaderResourceView* shader_resource_view_;
	ID3D11DepthStencilView* depth_stencil_view_;
	ID3D11RenderTargetView* render_target_view_;
	ID3D11UnorderedAccessView* unordered_access_view_;

	static uint32_t ToDXBindFlag(GPUTextureFlags value);
};

} // namespace aoe