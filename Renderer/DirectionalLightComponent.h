#pragma once

#include "Colors.h"
#include "DX11RenderDataComponents.h"
#include "DX11DirectionalLightPassFrameSystem.h"

namespace aoe {

class DX11DirectionalLightPassFrameSystem;

class DirectionalLightComponent {
private:
	friend class DX11DirectionalLightPassFrameSystem;

public:
	DirectionalLightComponent()
		: DirectionalLightComponent(Colors::kBlack)
	{}

	DirectionalLightComponent(Vector3f color)
		: color_(color)
	{
		UpdateColorData();
	}

	const Vector3f& GetColor() const {
		return color_;
	}

	void SetColor(Vector3f value) {
		color_ = value;
		UpdateColorData();
	}

	const DX11RenderData<Vector3fData>& GetColorData() const {
		return color_data_;
	}

	const DX11RenderData<Vector3fData>& GetTransformData() const {
		return transform_data_;
	}

private:
	Vector3f color_;
	DX11RenderData<Vector3fData> color_data_;
	DX11RenderData<Vector3fData> transform_data_;

	void UpdateColorData() {
		Vector3fData data{};
		data.value = color_;

		color_data_.Update(&data);
	}
};

} // namespace aoe