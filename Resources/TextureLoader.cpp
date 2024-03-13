#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "TextureLoader.h"

namespace aoe {

Image TextureLoader::Load(const std::string& path, uint32_t desired_channels) {
	int32_t x;
	int32_t y;
	int32_t comp;
	int32_t req_comp = static_cast<int32_t>(desired_channels);

	stbi_uc* data = stbi_load(path.c_str(), &x, &y, &comp, req_comp);

	if (data == nullptr) {
		throw new std::runtime_error("Failed to load image.");
	}

	uint32_t width = static_cast<uint32_t>(x);
	uint32_t height = static_cast<uint32_t>(y);
	uint32_t channels = desired_channels == 0 ? static_cast<uint32_t>(comp) : desired_channels;

	Image image(data, width, height, channels);
	stbi_image_free(data);

	return image;
}

} // namespace aoe