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

	GPUTexture2DDescription(
		uint32_t width, 
		uint32_t height, 
		GPUPixelFormat pixel_format, 
		GPUTextureFlags texture_flags
	)
		: width(width)
		, height(height)
		, pixel_format(pixel_format)
		, texture_flags(texture_flags)
	{}

	GPUTexture2DDescription()
		: width(0)
		, height(0)
		, pixel_format(GPUPixelFormat::kUnknown)
		, texture_flags(GPUTextureFlags::kNone)
	{}
};

class GPUTexture2D {
public:
	template<typename TElement>
	static GPUTexture2D Create(const GPUDevice& device, const GPUTexture2DDescription& description, const TElement* data);
	static GPUTexture2D Create(const GPUDevice& device, const GPUTexture2DDescription& description);

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

	GPUTexture2D(const GPUDevice& device, const GPUTexture2DDescription& description, const void* data, size_t stride);

private:
	const GPUDevice& device_;
	GPUTexture2DDescription description_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view_;
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> depth_stencil_view_;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> render_target_view_;
	Microsoft::WRL::ComPtr<ID3D11UnorderedAccessView> unordered_access_view_;

	static uint32_t ToDXBindFlag(GPUTextureFlags value);
};

template<typename TElement>
static GPUTexture2D GPUTexture2D::Create(const GPUDevice& device, const GPUTexture2DDescription& description, const TElement* data) {
	return { device, description, data, sizeof(TElement) };
}

} // namespace aoe