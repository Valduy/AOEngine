#include "pch.h"

#include <algorithm>

#include "DX11TextureScaler.h"

namespace aoe {

DX11TextureScaler::DX11TextureScaler(const GPUTexture2DDescription& description)
	: description_(description)
	, texture_(nullptr)
{
	Resize(description.width, description.height);
}

DX11TextureScaler::DX11TextureScaler(DX11TextureScaler&& other) noexcept
	: description_(other.description_)
	, texture_(nullptr)
{
	swap(*this, other);
}

DX11TextureScaler::~DX11TextureScaler() {
	delete texture_;
}

bool DX11TextureScaler::IsCollapsed() {
	return description_.width == 0 || description_.height == 0;
}

const DX11GPUTexture2D* DX11TextureScaler::GetTexture() {
	return texture_;
}

void DX11TextureScaler::Resize(uint32_t width, uint32_t height) {
	description_.width = width;
	description_.height = height;

	delete texture_;
	texture_ = IsCollapsed() ? nullptr : new DX11GPUTexture2D(description_);
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