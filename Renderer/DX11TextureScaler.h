#pragma once

#include "../Graphics/DX11GPUTexture2D.h"

namespace aoe {

class DX11TextureScaler {
public:
	DX11TextureScaler(const GPUTexture2DDescription& description)
		: description_(description)
		, texture_(nullptr)
	{
		Resize(description.width, description.height);
	}

	DX11TextureScaler(DX11TextureScaler&& other) noexcept
		: description_(other.description_)
		, texture_(nullptr)
	{
		swap(*this, other);
	}

	~DX11TextureScaler() {
		delete texture_;
	}

	bool IsCollapsed() {
		return description_.width == 0 || description_.height == 0;
	}

	const DX11GPUTexture2D* GetTexture() {
		return texture_;
	}

	void Resize(uint32_t width, uint32_t height) {
		description_.width = width;
		description_.height = height;

		delete texture_;
		texture_ = IsCollapsed() ? nullptr : new DX11GPUTexture2D(description_);
	}

	friend void swap(DX11TextureScaler& first, DX11TextureScaler& second) {
		using std::swap;

		swap(first.description_, second.description_);
		swap(first.texture_, second.texture_);
	}

	DX11TextureScaler& operator=(DX11TextureScaler other) {
		swap(*this, other);
		return *this;
	}

private:
	GPUTexture2DDescription description_;
	DX11GPUTexture2D* texture_;
};

} // namespace aoe