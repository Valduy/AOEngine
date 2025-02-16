#pragma once

#include "Colors.h"
#include "DX11RenderDataComponents.h"
#include "DX11PointLightPassFrameSystem.h"

namespace aoe {

class DX11PointLightPassFrameSystem;

class PointLightComponent {
private:
	friend class DX11PointLightPassFrameSystem;

public:
	PointLightComponent()
		: PointLightComponent(Colors::kBlack)
	{}

	PointLightComponent(Vector3f color)
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