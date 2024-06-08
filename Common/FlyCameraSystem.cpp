#include "pch.h"

#include "FlyCameraSystem.h"

namespace aoe {

FlyCameraSystem::FlyCameraSystem(const aoe::ServiceProvider& service_provider)
	: service_provider_(service_provider)
	, application_(nullptr)
	, world_(nullptr)
	, is_forward_(false)
	, is_backward_(false)
	, is_left_(false)
	, is_right_(false)
	, is_up_(false)
	, is_down_(false)
{}

void FlyCameraSystem::Initialize() {
	application_ = service_provider_.GetService<Application>();
	AOE_ASSERT_MSG(application_ != nullptr, "There is no Application service.");

	world_ = service_provider_.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");
}

void FlyCameraSystem::Terminate() {}

void FlyCameraSystem::PerTickUpdate(float dt) {
	CaptureInputs();

	Entity camera = CameraUtils::GetActualCamera(*world_);

	if (camera.IsNull() || !world_->Has<FlyCameraComponent>(camera)) {
		return;
	}

	auto transform_component = world_->Get<TransformComponent>(camera);
	auto fly_camera_component = world_->Get<FlyCameraComponent>(camera);

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
	Entity camera = CameraUtils::GetActualCamera(*world_);

	if (camera.IsNull()) {
		return;
	}

	auto transform_component = world_->Get<TransformComponent>(camera);
	auto fly_camera_component = world_->Get<FlyCameraComponent>(camera);

	transform_component->transform.position += GetMovement(transform_component, fly_camera_component->speed, dt);
}

void FlyCameraSystem::CaptureInputs() {
	is_forward_ = application_->GetInput().IsKeyHeld(Key::kW);
	is_backward_ = application_->GetInput().IsKeyHeld(Key::kS);
	is_left_ = application_->GetInput().IsKeyHeld(Key::kA);
	is_right_ = application_->GetInput().IsKeyHeld(Key::kD);
	is_up_ = application_->GetInput().IsKeyHeld(Key::kSpace);
	is_down_ = application_->GetInput().IsKeyHeld(Key::kShift);
}

Vector3f FlyCameraSystem::GetMovement(ComponentHandler<TransformComponent> transform_component, float speed, float dt) {
	Vector3f movement = Math::kZeros3f;

	if (is_forward_) {
		movement += transform_component->transform.GetForward();
	}
	if (is_backward_) {
		movement += transform_component->transform.GetBackward();
	}
	if (is_left_) {
		movement += transform_component->transform.GetLeft();
	}
	if (is_right_) {
		movement += transform_component->transform.GetRight();
	}
	if (is_up_) {
		movement += transform_component->transform.GetUp();
	}
	if (is_down_) {
		movement += transform_component->transform.GetDown();
	}

	return movement * speed * dt;
}

} // namespace aoe