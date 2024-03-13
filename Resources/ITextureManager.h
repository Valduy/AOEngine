#pragma once

#include <cstdint>
#include <string>

#include "Image.h"

namespace aoe {

using TextureId = uint32_t;

class ITextureManager {
public:
	static constexpr TextureId kEmpty = 0;

	virtual ~ITextureManager() = default;
	virtual TextureId Load(const std::string& path, uint32_t desired_channels = 0) = 0;
	virtual TextureId LoadR(const std::string& path) = 0;
	virtual TextureId LoadRG(const std::string& path) = 0;
	virtual TextureId LoadRGBA(const std::string& path) = 0;
	virtual TextureId Upload(Image image) = 0;
	virtual const Image& GetTexture(TextureId texture_id) = 0;
};

} // namespace aoe