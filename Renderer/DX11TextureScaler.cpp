#include "pch.h"

#include <algorithm>

#include "../Core/Debug.h"

#include "DX11TextureScaler.h"

namespace aoe {

DX11TextureScaler::DX11TextureScaler(const GPUTexture2DDescription& description)
	: description_(description)
	, texture_(nullptr)
	, width_(0)
	, height_(0)
{
	TryResize(description.width, description.height);
}

DX11TextureScaler::DX11TextureScaler(DX11TextureScaler&& other) noexcept
	: description_(other.description_)
	, texture_(nullptr)
	, width_(0)
	, height_(0)
{
	swap(*this, other);
}

DX11TextureScaler::~DX11TextureScaler() {
	delete texture_;
}

const DX11GPUTexture2D* DX11TextureScaler::GetTexture() {
	return texture_;
}

bool DX11TextureScaler::TryResize(uint32_t width, uint32_t height) {
	AOE_ASSERT_MSG(width > 0, "Width equal to zero.");
	AOE_ASSERT_MSG(height > 0, "Height equal to zero.");

	if (width_ == width && height_ == height) {
		return false;
	}

	width_ = width;
	height_ = height;

	description_.width = width_;
	description_.height = height_;

	delete texture_;
	texture_ = new DX11GPUTexture2D(description_);

	return true;
}

void swap(DX11TextureScaler& first, DX11TextureScaler& second) {
	using std::swap;

	swap(first.description_, second.description_);
	swap(first.texture_, second.texture_);
}

DX11TextureScaler& DX11TextureScaler::operator=(DX11TextureScaler other) {
	swap(*this, other);
	return *this;
}

} // namespace aoe