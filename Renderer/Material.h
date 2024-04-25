#pragma once

#include "../Core/Math.h"

namespace aoe {

struct Material {
	Vector3 diffuse = Math::kV3Zero;
	Vector3 specular = Math::kV3Zero;
	float shininess = 0.0f;

	Material(
		Vector3 diffuse, 
		Vector3 specular,
		float shininess
	)
		: diffuse(diffuse)
		, specular(specular)
		, shininess(shininess)
	{}

	Material() 
		: diffuse(Math::kV3Zero)
		, specular(Math::kV3Zero)
		, shininess(0.0f)
	{}
};

} // namespace aoe