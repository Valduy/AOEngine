#include "pch.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "../Core/FileHelper.h"
#include "../Application/Platform.h"

#include "TextureLoader.h"

namespace aoe {

Image TextureLoader::Load(const std::wstring& path, uint32_t desired_channels) {
	std::wstring full_path = std::format(L"{}/{}", Platform::GetExecutableDirectory(), path);
	std::vector<char> buffer = FileHelper::ReadAllFile(full_path);

	int32_t x, y, comp;
	stbi_uc* data = stbi_load_from_memory(
		reinterpret_cast<stbi_uc*>(buffer.data()),
		buffer.size(), &x, &y, &comp, 
		static_cast<int32_t>(desired_channels));

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