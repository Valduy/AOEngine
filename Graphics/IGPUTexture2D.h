#pragma once

#include <cstdint>

#include "GPUPixelFormat.h"
#include "GPUEnums.h"

namespace aoe {

class IGPUTextureView;

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

class IGPUTexture2D {
public:
	virtual void* GetNative() const = 0;
	virtual const GPUTexture2DDescription& GetDescription() const = 0;
	virtual const IGPUTextureView* GetTextureView() const = 0;

	virtual uint32_t GetWidth() const = 0;
	virtual uint32_t GetHeight() const = 0;

	virtual GPUPixelFormat GetPixelFormat() const = 0;

	virtual bool IsShaderResource() const = 0;
	virtual bool IsDepthStencil() const = 0;
	virtual bool IsRenderTarget() const = 0;
	virtual bool IsUnorderedAccess() const = 0;

	virtual ~IGPUTexture2D() = default;
};

} // namespace aoe