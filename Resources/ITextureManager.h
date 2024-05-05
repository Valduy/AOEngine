#pragma once

#include <string>

#include "Image.h"

namespace aoe {

class ITextureManager {
public:
	virtual ~ITextureManager() = default;
	virtual TextureId Load(const std::wstring& path, uint32_t desired_channels = 0) = 0;
	virtual TextureId LoadR(const std::wstring& path) = 0;
	virtual TextureId LoadRG(const std::wstring& path) = 0;
	virtual TextureId LoadRGBA(const std::wstring& path) = 0;
	virtual TextureId Upload(Image image) = 0;
	virtual TextureId GetDefault() = 0;
	virtual const Image& GetTexture(TextureId texture_id) = 0;
};

} // namespace aoe