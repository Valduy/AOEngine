#pragma once

#include "../Resources/Resources.h"

#include "Material.h"

namespace aoe {

struct RenderComponent {
	ModelId model_id;
	TextureId texture_id;
	Material material;

	RenderComponent(ModelId model_id, TextureId texture_id, Material material)
		: model_id(model_id)
		, texture_id(texture_id)
		, material(std::move(material))
	{}
};

} // namespace aoe