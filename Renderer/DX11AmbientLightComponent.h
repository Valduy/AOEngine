#pragma once

#include "Colors.h"
#include "DX11RenderData.h"
#include "DX11BufferModels.h"

namespace aoe {

class DX11AmbientLightComponent {
public:
	DX11AmbientLightComponent()
		: DX11AmbientLightComponent(Colors::kBlack)
	{}

	DX11AmbientLightComponent(Vector3f color)
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