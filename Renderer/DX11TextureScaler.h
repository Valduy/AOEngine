#pragma once

#include "../Graphics/DX11GPUTexture2D.h"

namespace aoe {

class DX11TextureScaler {
public:
	DX11TextureScaler(const GPUTexture2DDescription& description);
	DX11TextureScaler(DX11TextureScaler&& other) noexcept;

	~DX11TextureScaler();

	const DX11GPUTexture2D* GetTexture();

	bool TryResize(uint32_t width, uint32_t height);

	friend void swap(DX11TextureScaler& first, DX11TextureScaler& second);

	DX11TextureScaler& operator=(DX11TextureScaler other);

private:
	GPUTexture2DDescription description_;
	DX11GPUTexture2D* texture_;

	int32_t width_;
	int32_t height_;
};

} // namespace aoe