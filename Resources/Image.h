#pragma once

#include <cstdint>
#include <cstring>
#include <algorithm>

namespace aoe {

class Image {
public:
	Image(const uint8_t* data, uint32_t width, uint32_t height, uint32_t channels);
	Image();
	Image(Image&& other) noexcept;

	~Image();

	uint32_t GetWidth() const;
	uint32_t GetHeight() const;
	uint32_t GetChannels() const;
	const uint8_t* GetData() const;

	friend void swap(Image& first, Image& second);

	uint8_t operator[](size_t index);
	Image& operator=(Image other);

private:
	uint32_t width_;
	uint32_t height_;
	uint32_t channels_;

	uint8_t* data_;
};

} // namespace aoe