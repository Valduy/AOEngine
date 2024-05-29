#include "pch.h"

#include "CameraUtils.h"

namespace aoe {

Entity CameraUtils::GetActualCamera(World& world) {
	Entity camera = Entity::Null();

	world.ForEach<TransformComponent, CameraComponent>(
	[&](auto entity, auto transform_component, auto camera_component) {
		camera = entity;
	});

	return camera;
}

Matrix4f CameraUtils::GetProjectionMatrix(World& world, Entity camera) {
	auto camera_component = world.Get<CameraComponent>(camera);
	return camera_component->GetProjectionMatrix();
}

Matrix4f CameraUtils::GetViewMatrix(World& world, Entity camera) {
	auto transform_component = world.Get<TransformComponent>(camera);
	Vector3f forward = transform_component->GetForward();
	Vector3f position = transform_component->position;

	return Matrix4f::LookAt(forward, position, Math::kUp, Math::kLH);
}

Matrix4f CameraUtils::GetCameraMatrix(World& world, Entity camera) {
	Matrix4f view_matrix = GetViewMatrix(world, camera);
	Matrix4f projection_matrix = GetProjectionMatrix(world, camera);
	Matrix4f camera_matrix = projection_matrix * view_matrix;

	return camera_matrix;
}

} // namespace aoe