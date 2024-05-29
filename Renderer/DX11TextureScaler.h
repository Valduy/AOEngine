#pragma once

#include "../Graphics/DX11GPUTexture2D.h"

namespace aoe {

class DX11TextureScaler {
public:
	DX11TextureScaler(const GPUTexture2DDescription& description);
	DX11TextureScaler(DX11TextureScaler&& other) noexcept;

	~DX11TextureScaler();

	bool IsCollapsed();
	const DX11GPUTexture2D* GetTexture();

	void Resize(uint32_t width, uint32_t height);

	friend void swap(DX11TextureScaler& first, DX11TextureScaler& second);

	DX11TextureScaler& operator=(DX11TextureScaler other);

private:
	GPUTexture2DDescription description_;
	DX11GPUTexture2D* texture_;
};

} // namespace aoe