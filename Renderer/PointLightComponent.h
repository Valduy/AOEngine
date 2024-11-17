#pragma once

#include "../Core/Math.h"

namespace aoe {

struct PointLightComponent {
	Vector3f color;

	PointLightComponent()
		: color(0.0f, 0.0f, 0.0f)
	{}

	PointLightComponent(Vector3f color)
		: color(color)
	{}
};

} // namespace aoe