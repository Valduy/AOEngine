#pragma once

#include "../Resources/Resources.h"

#include "Material.h"
#include "DX11RenderDataComponents.h"

namespace aoe {

class RenderComponent {
public:
	RenderComponent(ModelId model_id, TextureId texture_id, Material material)
		: model_id_(model_id)
		, texture_id_(texture_id)
		, material_(std::move(material))
	{
		UpdateMaterialData();
	}

	ModelId GetModelId() const {
		return model_id_;
	}

	void SetModelId(ModelId value) {
		model_id_ = value;
	}

	TextureId GetTextureId() const {
		return texture_id_;
	}

	void SetTextureId(TextureId value) {
		texture_id_ = value;
	}

	const Material& GetMaterial() const {
		return material_;
	}

	void SetMaterial(Material value) {
		material_ = std::move(value);
		UpdateMaterialData();
	}

	const DX11RenderData<MaterialData>& GetMaterialData() const {
		return material_data_;
	}

private:
	ModelId model_id_;
	TextureId texture_id_;
	Material material_;
	DX11RenderData<MaterialData> material_data_;

	void UpdateMaterialData() {
		MaterialData data{};
		data.diffuse = material_.diffuse;
		data.specular = material_.specular;
		data.shininess = material_.shininess;

		material_data_.Update(&data);
	}
};

} // namespace aoe