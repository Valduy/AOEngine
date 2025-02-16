#pragma once

#include "Colors.h"
#include "DX11RenderData.h"
#include "DX11BufferModels.h"
#include "DX11PointLightPassSystem.h"

namespace aoe {

class DX11PointLightPassSystem;

class DX11PointLightComponent {
private:
	friend class DX11PointLightPassSystem;

public:
	DX11PointLightComponent()
		: DX11PointLightComponent(Colors::kBlack)
	{}

	DX11PointLightComponent(Vector3f color)
		: color_(color)
	{
		UpdateColorData();
	}

	const Vector3f GetColor() const {
		return color_;
	}

	void SetColor(Vector3f value) {
		color_ = value;
		UpdateColorData();
	}

	const DX11RenderData<Vector3fData>& GetColorData() const {
		return color_data_;
	}

	const DX11RenderData<PointLightTransformData>& GetTransformData() const {
		return transform_data_;
	}

private:
	Vector3f color_;
	DX11RenderData<Vector3fData> color_data_;
	DX11RenderData<PointLightTransformData> transform_data_;

	void UpdateColorData() {
		Vector3fData data{};
		data.value = color_;

		color_data_.Update(&data);
	}
};

} // namespace aoe