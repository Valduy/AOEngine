#pragma once

#include <unordered_map>

#include "../Graphics/DX11GPUTexture2D.h"

#include "Resources.h"
#include "ITextureManager.h"
#include "TextureLoader.h"

namespace aoe {

class DX11TextureManager : public ITextureManager {
public:
	static constexpr TextureId kDefault = 0;

	DX11TextureManager();

	TextureId Load(const std::wstring& path, uint32_t desired_channels = 0) override;
	TextureId LoadR(const std::wstring& path) override;
	TextureId LoadRG(const std::wstring& path) override;
	TextureId LoadRGBA(const std::wstring& path) override;
	TextureId Upload(Image image) override;
	TextureId GetDefault() override;
	const Image& GetTexture(TextureId texture_id) override;
	const DX11GPUTexture2D& GetTextureResources(TextureId texture_id);

private:
	std::unordered_map<std::wstring, TextureId> path_to_texture_id_;
	std::vector<Image> images_;
	std::vector<DX11GPUTexture2D> textures_resources_;

	static DX11GPUTexture2D CreateTexture(Image& image);
	static GPUPixelFormat GetPixelFormat(Image& image);
};

} // namespace aoe