#pragma once

#include "GPUPixelFormat.h"
#include "GPUEnums.h"
#include "DX11GPUDevice.h"
#include "DX11GPUTextureView.h"

namespace aoe {

struct GPUTexture2DDescription {
	uint32_t width;
	uint32_t height;
	GPUPixelFormat pixel_format;
	GPUTextureFlags texture_flags;

	static GPUTexture2DDescription DepthStencilBuffer(uint32_t width, uint32_t height) {
		return { width, height, GPUPixelFormat::kD24_Unorm_S8_Uint, GPUTextureFlags::kDepthStencil };
	}

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

class DX11GPUTexture2D {
public:
	template<typename TElement>
	static DX11GPUTexture2D Create(const GPUTexture2DDescription& description, const TElement* data);

	ID3D11Texture2D* GetNative() const;
	const GPUTexture2DDescription& GetDescription() const;
	virtual const DX11GPUTextureView& GetTextureView() const;

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;

	GPUPixelFormat GetPixelFormat() const;

	bool IsShaderResource() const;
	bool IsDepthStencil() const;
	bool IsRenderTarget() const;
	bool IsUnorderedAccess() const;

	DX11GPUTexture2D(const GPUTexture2DDescription& description, const void* data, uint32_t stride);
	DX11GPUTexture2D(const GPUTexture2DDescription& description);
	DX11GPUTexture2D(ID3D11Texture2D* texture);

private:
	GPUTexture2DDescription description_;
	DX11GPUTextureView texture_view_;
	Microsoft::WRL::ComPtr<ID3D11Texture2D> texture_;
	
	static uint32_t ToDXBindFlag(GPUTextureFlags value);
	static GPUTextureFlags FromDXBindFlag(uint32_t value);

	void CreateTextureViews();
};

template<typename TElement>
static DX11GPUTexture2D DX11GPUTexture2D::Create(const GPUTexture2DDescription& description, const TElement* data) {
	return { description, data, sizeof(TElement) };
}

} // namespace aoe