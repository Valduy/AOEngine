#include "pch.h"

#include <algorithm>

#include "Image.h"

namespace aoe {

Image::Image(const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels)
	: width_(width)
	, height_(height)
	, channels_(channels)
	, data_(nullptr)
{
	size_t size = channels_ * width_ * height_;
	data_ = new uint8_t[size];
	memcpy(data_, data, size);
}

Image::Image()
	: Image(nullptr, 0, 0, 0)
{}

Image::Image(Image&& other) noexcept
	: Image()
{
	swap(*this, other);
}

Image::~Image() {
	delete data_;
}

uint32_t Image::GetWidth() const {
	return width_;
}

uint32_t Image::GetHeight() const {
	return height_;
}

uint32_t Image::GetChannels() const {
	return channels_;
}

const uint8_t* Image::GetData() const {
	return data_;
}

void swap(Image& first, Image& second) {
	using std::swap;

	swap(first.width_, second.width_);
	swap(first.height_, second.height_);
	swap(first.channels_, second.channels_);
	swap(first.data_, second.data_);
}

uint8_t Image::operator[](size_t index) {
	return data_[index];
}

Image& Image::operator=(Image other) {
	swap(*this, other);
	return *this;
}

} // namespace aoe