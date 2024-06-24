#pragma once

#include "../ECS/World.h"
#include "../Game/TransformComponent.h"

#include "CameraComponent.h"

namespace aoe {

class CameraUtils {
public:
	CameraUtils() = delete;

	static bool HasCamera(World& world);
	static Entity GetActualCamera(World& world);
	static Matrix4f GetProjectionMatrix(World& world, Entity camera);
	static Matrix4f GetViewMatrix(World& world, Entity camera);
	static Matrix4f GetCameraMatrix(World& world, Entity camera);
};

} // namespace aoe