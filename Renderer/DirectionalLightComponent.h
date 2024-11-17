#pragma once

#include "../Core/Math.h"

namespace aoe {

struct DirectionalLightComponent {
	Vector3f color;

	DirectionalLightComponent()
		: color(0.0f, 0.0f, 0.0f)
	{}

	DirectionalLightComponent(Vector3f color)
		: color(color)
	{}
};

} // namespace aoe