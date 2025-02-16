#pragma once

#include "Colors.h"
#include "DX11RenderDataComponents.h"

namespace aoe {

class AmbientLightComponent {
public:
	AmbientLightComponent()
		: AmbientLightComponent(Colors::kBlack)
	{}

	AmbientLightComponent(Vector3f color)
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

private:
	Vector3f color_;
	DX11RenderData<Vector3fData> color_data_;

	void UpdateColorData() {
		Vector3fData data{};
		data.value = color_;

		color_data_.Update(&data);
	}
};

} // namespace aoe