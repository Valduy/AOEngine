#include "pch.h"

#include "FlyCameraFrameSystem.h"

namespace aoe {

void FlyCameraFrameSystem::PerFrameUpdate(float dt) {
	Entity camera = CameraUtils::GetActualCamera(*GetWorld());

	if (camera.IsNull() || !GetWorld()->Has<FlyCameraComponent>(camera)) {
		return;
	}

	auto transform_component = GetWorld()->Get<TransformComponent>(camera);
	auto fly_camera_component = GetWorld()->Get<FlyCameraComponent>(camera);

	Vector3f movement = GetMovement(transform_component->transform, fly_camera_component, dt);
	transform_component->transform.position += movement;
}

Vector3f FlyCameraFrameSystem::GetMovement(const Transform& transform, CH<FlyCameraComponent> fly_camera_component, float dt) {
	Vector3f movement = Math::kZeros3f;

	const float x_axis = static_cast<float>(Math::Sign(fly_camera_component->axis.x));
	const float y_axis = static_cast<float>(Math::Sign(fly_camera_component->axis.y));
	const float z_axis = static_cast<float>(Math::Sign(fly_camera_component->axis.z));

	movement += transform.GetRight() * x_axis;
	movement += transform.GetUp() * y_axis;
	movement += transform.GetForward() * z_axis;

	return Math::Equal(movement.Length(), 0)
		? Math::kZeros3f
		: movement.Normalized() * fly_camera_component->speed * dt;
}

} // namespace aoe