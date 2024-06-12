#include "pch.h"

#include "FlyCameraSystem.h"

namespace aoe {

FlyCameraSystem::FlyCameraSystem()
	: application_(nullptr)
	, is_forward_(false)
	, is_backward_(false)
	, is_left_(false)
	, is_right_(false)
	, is_up_(false)
	, is_down_(false)
{}

void FlyCameraSystem::Initialize(const aoe::ServiceProvider& service_provider) {
	ECSSystemBase::Initialize(service_provider);

	application_ = service_provider.GetService<Application>();
	AOE_ASSERT_MSG(application_ != nullptr, "There is no Application service.");
}

void FlyCameraSystem::PerTickUpdate(float dt) {
	CaptureInputs();

	Entity camera = CameraUtils::GetActualCamera(*GetWorld());

	if (camera.IsNull() || !GetWorld()->Has<FlyCameraComponent>(camera)) {
		return;
	}

	auto transform_component = GetWorld()->Get<TransformComponent>(camera);
	auto fly_camera_component = GetWorld()->Get<FlyCameraComponent>(camera);

	const int dx = application_->GetInput().GetMouseDelta().x;
	const int dy = application_->GetInput().GetMouseDelta().y;

	float yaw = fly_camera_component->yaw + fly_camera_component->sensitivity * dx;
	float pitch = fly_camera_component->pitch + fly_camera_component->sensitivity * dy;
	pitch = Math::Clamp(pitch, -Math::kPiDiv2 + kEpsilon, Math::kPiDiv2 - kEpsilon);

	fly_camera_component->yaw = yaw;
	fly_camera_component->pitch = pitch;
	transform_component->transform.rotation = Quaternion::FromEulerAngles(pitch, yaw, 0);
}

void FlyCameraSystem::PerFrameUpdate(float dt) {
	Entity camera = CameraUtils::GetActualCamera(*GetWorld());

	if (camera.IsNull()) {
		return;
	}

	auto transform_component = GetWorld()->Get<TransformComponent>(camera);
	auto fly_camera_component = GetWorld()->Get<FlyCameraComponent>(camera);

	Vector3f movement = GetMovement(transform_component->transform, fly_camera_component->speed, dt);
	transform_component->transform.position += movement;
}

void FlyCameraSystem::CaptureInputs() {
	is_forward_ = application_->GetInput().IsKeyHeld(Key::kW);
	is_backward_ = application_->GetInput().IsKeyHeld(Key::kS);
	is_left_ = application_->GetInput().IsKeyHeld(Key::kA);
	is_right_ = application_->GetInput().IsKeyHeld(Key::kD);
	is_up_ = application_->GetInput().IsKeyHeld(Key::kSpace);
	is_down_ = application_->GetInput().IsKeyHeld(Key::kShift);
}

Vector3f FlyCameraSystem::GetMovement(Transform transform, float speed, float dt) {
	Vector3f movement = Math::kZeros3f;

	if (is_forward_) {
		movement += transform.GetForward();
	}
	if (is_backward_) {
		movement += transform.GetBackward();
	}
	if (is_left_) {
		movement += transform.GetLeft();
	}
	if (is_right_) {
		movement += transform.GetRight();
	}
	if (is_up_) {
		movement += transform.GetUp();
	}
	if (is_down_) {
		movement += transform.GetDown();
	}

	return movement * speed * dt;
}

} // namespace aoe