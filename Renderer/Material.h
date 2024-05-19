#pragma once

#include "../Core/Math.h"

namespace aoe {

struct Material {
	Vector3f diffuse = Math::kZeros3f;
	Vector3f specular = Math::kZeros3f;
	float shininess = 0.0f;

	Material(
		Vector3f diffuse, 
		Vector3f specular,
		float shininess
	)
		: diffuse(diffuse)
		, specular(specular)
		, shininess(shininess)
	{}

	Material() 
		: diffuse(Math::kZeros3f)
		, specular(Math::kZeros3f)
		, shininess(0.0f)
	{}
};

} // namespace aoe