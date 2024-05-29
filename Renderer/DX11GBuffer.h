#pragma once

#include "DX11TextureScaler.h"

namespace aoe {

class DX11GBuffer {
public:
	DX11GBuffer(uint32_t width, uint32_t height);

	const DX11GPUTexture2D* GetDiffuseTexture();
	const DX11GPUTexture2D* GetSpecularTexture();
	const DX11GPUTexture2D* GetNormalTexture();
	const DX11GPUTexture2D* GetPositionTexture();
	const DX11GPUTexture2D* GetAccumulatorTexture();

	void Resize(uint32_t width, uint32_t height);

private:
	DX11TextureScaler diffuse_;
	DX11TextureScaler specular_;
	DX11TextureScaler normal_;
	DX11TextureScaler position_;
	DX11TextureScaler accumulator_;

	static GPUTexture2DDescription GetTexture8x4Description(uint32_t width, uint32_t height);
	static GPUTexture2DDescription GetTexture32x4Description(uint32_t width, uint32_t height);
};

} // namespace aoe