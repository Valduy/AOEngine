#pragma once

#include "../Core/Math.h"

namespace aoe {

struct AmbientLightComponent {
	Vector3f color;
	float intensity;

	AmbientLightComponent()
		: color(0.0f, 0.0f, 0.0f)
		, intensity(0.0f)
	{}

	AmbientLightComponent(Vector3f color, float intensity)
		: color(color)
		, intensity(intensity)
	{}
};

} // namespace aoe