#pragma once

#include <unordered_map>

#include "../Graphics/DX11GPUTexture2D.h"

#include "ITextureManager.h"
#include "TextureLoader.h"

namespace aoe {

class DX11TextureManager : public ITextureManager {
public:
	DX11TextureManager()
		: path_to_texture_id_()
		, images_()
		, textures_resources_()
	{}

	TextureId Load(const std::string& path, uint32_t desired_channels = 0) override {
		auto it = path_to_texture_id_.find(path);

		if (it != path_to_texture_id_.end()) {
			return it->second;
		}

		TextureId id = Upload(TextureLoader::Load(path, desired_channels));
		path_to_texture_id_[path] = id;
		return id;
	}

	TextureId LoadR(const std::string& path) {
		return Load(path, 1);
	}

	TextureId LoadRG(const std::string& path) {
		return Load(path, 2);
	}

	TextureId LoadRGBA(const std::string& path) {
		return Load(path, 4);
	}

	TextureId Upload(Image image) override {
		images_.emplace_back(std::move(image));
		textures_resources_.emplace_back(CreateTexture(images_.back()));
		return static_cast<TextureId>(images_.size() - 1);
	}

	const Image& GetTexture(TextureId texture_id) override {
		return images_[texture_id];
	}

	const DX11GPUTexture2D& GetTextureResources(TextureId texture_id) {
		return textures_resources_[texture_id];
	}

private:
	std::unordered_map<std::string, TextureId> path_to_texture_id_;

	std::vector<Image> images_;
	std::vector<DX11GPUTexture2D> textures_resources_;

	static DX11GPUTexture2D CreateTexture(Image& image) {
		const GPUTexture2DDescription texture_desc{
			image.GetWidth(),
			image.GetHeight(),
			GetPixelFormat(image),
			GPUTextureFlags::kShaderResource,
		};
	}

	static GPUPixelFormat GetPixelFormat(Image& image) {
		switch (image.GetChannels())
		{
		case 1:
			return GPUPixelFormat::kR8_Unorm;
		case 2:
			return GPUPixelFormat::kR8G8_Unorm;
		case 4:
			return GPUPixelFormat::kR8G8B8A8_Unorm;
		default:
			return GPUPixelFormat::kUnknown;
		}
	}
};

} // namespace aoe