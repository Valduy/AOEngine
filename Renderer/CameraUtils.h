#pragma once

#include "../ECS/World.h"
#include "../Game/TransformComponent.h"

#include "CameraComponent.h"

namespace aoe {

class CameraUtils {
public:
	CameraUtils() = delete;

	static Entity GetActualCamera(World& world) {
		Entity camera = Entity::Null();

		world.ForEach<TransformComponent, CameraComponent>(
		[&](auto entity, auto transform_component, auto camera_component) {
			camera = entity;
		});

		return camera;
	}

	static Matrix4 GetProjectionMatrix(World& world, Entity camera) {
		auto camera_component = world.Get<CameraComponent>(camera);
		return camera_component->GetProjectionMatrix();
	}

	static Matrix4 GetViewMatrix(World& world, Entity camera) {
		auto transform_component = world.Get<TransformComponent>(camera);
		Vector3 forward = transform_component->GetForward();
		Vector3 position = transform_component->position;

		return Matrix4::LookAt(forward, position, Math::kUp, Math::kLH);
	}

	static Matrix4 GetCameraMatrix(World& world, Entity camera) {
		Matrix4 view_matrix = GetViewMatrix(world, camera);
		Matrix4 projection_matrix = GetProjectionMatrix(world, camera);
		Matrix4 camera_matrix = projection_matrix * view_matrix;

		return camera_matrix;
	}
};

} // namespace aoe