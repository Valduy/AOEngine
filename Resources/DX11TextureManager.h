#pragma once

#include <unordered_map>

#include "../Graphics/DX11GPUTexture2D.h"

#include "ITextureManager.h"
#include "TextureLoader.h"

namespace aoe {

class DX11TextureManager : public ITextureManager {
public:
	DX11TextureManager();

	TextureId Load(const std::string& path, uint32_t desired_channels = 0) override;
	TextureId LoadR(const std::string& path);
	TextureId LoadRG(const std::string& path);
	TextureId LoadRGBA(const std::string& path);
	TextureId Upload(Image image) override;
	const Image& GetTexture(TextureId texture_id) override;
	const DX11GPUTexture2D& GetTextureResources(TextureId texture_id);

private:
	std::unordered_map<std::string, TextureId> path_to_texture_id_;

	std::vector<Image> images_;
	std::vector<DX11GPUTexture2D> textures_resources_;

	static DX11GPUTexture2D CreateTexture(Image& image);
	static GPUPixelFormat GetPixelFormat(Image& image);
};

} // namespace aoe