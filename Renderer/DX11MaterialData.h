#pragma once

#include "../Core/Math.h"

namespace aoe {

struct DX11MaterialData {
	Vector3 ambient = Math::kV3Zero;
	float dummy0;
	Vector3 diffuse = Math::kV3Zero;
	float dummy1;
	Vector3 specular = Math::kV3Zero;
	float shininess = 0.0f;
};

} // namespace aoe