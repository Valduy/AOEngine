#pragma once

#include "../Core/Math.h"

namespace aoe {

struct FlyCameraComponent {
	Vector3f axis;
	Vector3f angles; // x: pitch, y: yaw, z: roll
	float speed;
	float sensitivity;
};

} // namespace aoe