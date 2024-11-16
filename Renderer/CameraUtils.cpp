#include "pch.h"

#include "CameraUtils.h"

namespace aoe {

bool CameraUtils::HasCamera(World& world) {
	Entity camera = GetActualCamera(world);
	return !camera.IsNull();
}

Entity CameraUtils::GetActualCamera(World& world) {
	for (Entity camera : world.FilterEntities<TransformComponent, CameraComponent>()) {
		return camera;
	}
	
	return Entity::Null();
}

Matrix4f CameraUtils::GetProjectionMatrix(World& world, Entity camera) {
	auto camera_component = world.GetComponent<CameraComponent>(camera);
	return camera_component->GetProjectionMatrix();
}

Matrix4f CameraUtils::GetViewMatrix(
	World& world, 
	Relationeer<TransformComponent>& relationeer, 
	Entity camera) 
{
	auto global_transform = TransformUtils::GetGlobalTransform(world, relationeer, camera);
	Vector3f forward = global_transform.GetForward();
	Vector3f position = global_transform.position;

	return Matrix4f::LookAt(position + forward, position, Math::kUp, Math::kLH);
}

Matrix4f CameraUtils::GetCameraMatrix(
	World& world, 
	Relationeer<TransformComponent>& relationeer, 
	Entity camera) 
{
	Matrix4f view_matrix = GetViewMatrix(world, relationeer, camera);
	Matrix4f projection_matrix = GetProjectionMatrix(world, camera);
	Matrix4f camera_matrix = projection_matrix * view_matrix;

	return camera_matrix;
}

} // namespace aoe