#include "pch.h"

#include "DX11TextureManager.h"

namespace aoe {

DX11TextureManager::DX11TextureManager()
	: path_to_texture_id_()
	, images_()
	, textures_resources_()
{}

TextureId DX11TextureManager::Load(const std::string& path, uint32_t desired_channels) {
	auto it = path_to_texture_id_.find(path);

	if (it != path_to_texture_id_.end()) {
		return it->second;
	}

	TextureId id = Upload(TextureLoader::Load(path, desired_channels));
	path_to_texture_id_[path] = id;
	return id;
}

TextureId DX11TextureManager::LoadR(const std::string& path) {
	return Load(path, 1);
}

TextureId DX11TextureManager::LoadRG(const std::string& path) {
	return Load(path, 2);
}

TextureId DX11TextureManager::LoadRGBA(const std::string& path) {
	return Load(path, 4);
}

TextureId DX11TextureManager::Upload(Image image) {
	images_.emplace_back(std::move(image));
	textures_resources_.emplace_back(CreateTexture(images_.back()));
	return static_cast<TextureId>(images_.size() - 1);
}

const Image& DX11TextureManager::GetTexture(TextureId texture_id) {
	return images_[texture_id];
}

const DX11GPUTexture2D& DX11TextureManager::GetTextureResources(TextureId texture_id) {
	return textures_resources_[texture_id];
}

DX11GPUTexture2D DX11TextureManager::CreateTexture(Image& image) {
	const GPUTexture2DDescription texture_desc{
		image.GetWidth(),
		image.GetHeight(),
		GetPixelFormat(image),
		GPUTextureFlags::kShaderResource,
	};

	return { texture_desc, image.GetData(), image.GetChannels() };
}

GPUPixelFormat DX11TextureManager::GetPixelFormat(Image& image) {
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

} // namespace aoe