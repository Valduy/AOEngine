#pragma once

#include "../Core/Math.h"

namespace aoe {

struct Material {
	Vector3 ambient = Math::kV3Zero;
	Vector3 diffuse = Math::kV3Zero;
	Vector3 specular = Math::kV3Zero;
	float shininess = 0.0f;

	Material(
		Vector3 ambient, 
		Vector3 diffuse, 
		Vector3 specular,
		float shininess
	)
		: ambient(ambient)
		, diffuse(diffuse)
		, specular(specular)
		, shininess(shininess)
	{}

	Material() 
		: ambient(Math::kV3Zero)
		, diffuse(Math::kV3Zero)
		, specular(Math::kV3Zero)
		, shininess(0.0f)
	{}
};

} // namespace aoe