#pragma once

#include "../Resources/Resources.h"

#include "Material.h"
#include "DX11RenderData.h"
#include "DX11BufferModels.h"
#include "DX11GeometryPassSystem.h"

namespace aoe {

class DX11GeometryPassSystem;

class DX11RenderComponent {
private:
	friend class DX11GeometryPassSystem;

public:
	DX11RenderComponent(ModelId model_id, TextureId texture_id, Material material)
		: model_id_(model_id)
		, texture_id_(texture_id)
		, material_(std::move(material))
		, material_data_()
		, transform_data_()
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

	const DX11RenderData<TransformData>& GetTransformData() const {
		return transform_data_;
	}

private:
	ModelId model_id_;
	TextureId texture_id_;
	Material material_;
	DX11RenderData<MaterialData> material_data_;
	DX11RenderData<TransformData> transform_data_;

	void UpdateMaterialData() {
		MaterialData data{};
		data.diffuse = material_.diffuse;
		data.specular = material_.specular;
		data.shininess = material_.shininess;

		material_data_.Update(&data);
	}
};

} // namespace aoe