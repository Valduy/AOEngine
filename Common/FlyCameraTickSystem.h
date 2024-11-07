#pragma once

#include "../Game/ECSSystemBase.h"
#include "../Application/Application.h"

#include "FlyCameraComponent.h"

namespace aoe {

class FlyCameraTickSystem : public ECSSystemBase {
public:
	void Initialize(const aoe::ServiceProvider& service_provider);
	void Update(float dt) override;

private:
	static constexpr float kEpsilon = 0.001f;
	static constexpr float kMinPitch = -Math::kPiDiv2 + kEpsilon;
	static constexpr float kMaxPitch = Math::kPiDiv2 - kEpsilon;

	Application* application_;

	void UpdateAxis(CH<FlyCameraComponent> fly_camera_component);
	void UpdateAngles(CH<FlyCameraComponent> fly_camera_component);
};

} // namespace aoe