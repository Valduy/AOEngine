#include "pch.h"

#include "../Game/TransformComponent.h"

#include "FlyCameraTickSystem.h"

namespace aoe {

void FlyCameraTickSystem::Initialize(const aoe::ServiceProvider& service_provider) {
	ECSSystemBase::Initialize(service_provider);

	application_ = service_provider.TryGetService<Application>();
	AOE_ASSERT_MSG(application_ != nullptr, "There is no Application service.");
}

void FlyCameraTickSystem::Update(float dt) {
	for (Entity camera : FilterEntities<TransformComponent, FlyCameraComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(camera);
		auto fly_camera_component = GetComponent<FlyCameraComponent>(camera);

		UpdateAxis(fly_camera_component);
		UpdateAngles(fly_camera_component);

		Quaternion rotation = Quaternion::FromEulerAngles(fly_camera_component->angles);
		transform_component->SetRotation(rotation);
	}
}

void FlyCameraTickSystem::UpdateAxis(CH<FlyCameraComponent> fly_camera_component) {
	fly_camera_component->axis = Math::kZeros3f;

	if (application_->GetInput().IsKeyHeld(Key::kW)) {
		fly_camera_component->axis += Math::kForward;
	}
	if (application_->GetInput().IsKeyHeld(Key::kS)) {
		fly_camera_component->axis += Math::kBackward;
	}
	if (application_->GetInput().IsKeyHeld(Key::kA)) {
		fly_camera_component->axis += Math::kLeft;
	}
	if (application_->GetInput().IsKeyHeld(Key::kD)) {
		fly_camera_component->axis += Math::kRight;
	}
	if (application_->GetInput().IsKeyHeld(Key::kSpace)) {
		fly_camera_component->axis += Math::kUp;
	}
	if (application_->GetInput().IsKeyHeld(Key::kShift)) {
		fly_camera_component->axis += Math::kDown;
	}
}

void FlyCameraTickSystem::UpdateAngles(CH<FlyCameraComponent> fly_camera_component) {
	const float dy = fly_camera_component->sensitivity * application_->GetInput().GetMouseDelta().x;
	const float dx = fly_camera_component->sensitivity * application_->GetInput().GetMouseDelta().y;

	const float pitch = Math::Clamp(fly_camera_component->angles.x + dx, kMinPitch, kMaxPitch);
	const float yaw = fly_camera_component->angles.y + dy;
	const float roll = 0;

	fly_camera_component->angles = { pitch, yaw, roll };
}

} // namespace aoe