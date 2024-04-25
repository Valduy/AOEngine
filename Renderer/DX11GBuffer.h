#pragma once

#include "DX11TextureScaler.h"

namespace aoe {

class DX11GBuffer {
public:
	DX11GBuffer(uint32_t width, uint32_t height)
		: diffuse_(GetTexture8x4Description(width, height))
		, specular_(GetTexture32x4Description(width, height))
		, normal_(GetTexture32x4Description(width, height))
		, position_(GetTexture32x4Description(width, height))
		, accumulator_(GetTexture32x4Description(width, height))
	{
		Resize(width, height);
	}

	const DX11GPUTexture2D* GetDiffuseTexture() {
		return diffuse_.GetTexture();
	}

	const DX11GPUTexture2D* GetSpecularTexture() {
		return specular_.GetTexture();
	}

	const DX11GPUTexture2D* GetNormalTexture() {
		return normal_.GetTexture();
	}

	const DX11GPUTexture2D* GetPositionTexture() {
		return position_.GetTexture();
	}

	const DX11GPUTexture2D* GetAccumulatorTexture() {
		return accumulator_.GetTexture();
	}

	void Resize(uint32_t width, uint32_t height) {
		diffuse_.Resize(width, height);
		normal_.Resize(width, height);
		position_.Resize(width, height);
		accumulator_.Resize(width, height);
	}

private:
	DX11TextureScaler diffuse_;
	DX11TextureScaler specular_;
	DX11TextureScaler normal_;
	DX11TextureScaler position_;
	DX11TextureScaler accumulator_;

	static GPUTexture2DDescription GetTexture8x4Description(uint32_t width, uint32_t height) {
		return {
			width,
			height,
			GPUPixelFormat::kR8G8B8A8_Unorm,
			GPUTextureFlags::kRenderTarget | GPUTextureFlags::kShaderResource
		};
	}

	static GPUTexture2DDescription GetTexture32x4Description(uint32_t width, uint32_t height) {
		return {
			width,
			height,
			GPUPixelFormat::kR32G32B32A32_Float,
			GPUTextureFlags::kRenderTarget | GPUTextureFlags::kShaderResource
		};
	}
};

} // namespace aoe