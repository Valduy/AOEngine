#pragma once

#include "../Game/ECSSystemBase.h"
#include "../Game/ServiceProvider.h"
#include "../Application/Application.h"
#include "../ECS/World.h"
#include "../Renderer/CameraUtils.h"

#include "FlyCameraComponent.h"

namespace aoe {

class FlyCameraSystem : public ECSSystemBase {
public:
	FlyCameraSystem();

	void Initialize(const aoe::ServiceProvider& service_provider) override;
	
	void PerTickUpdate(float dt) override;
	void PerFrameUpdate(float dt) override;

private:
	static constexpr float kEpsilon = 0.001f;

	Application* application_;

	bool is_forward_;
	bool is_backward_;
	bool is_left_;
	bool is_right_;
	bool is_up_;
	bool is_down_;
	
	void CaptureInputs();
	Vector3f GetMovement(Transform transform, float speed, float dt);
};

} // namespace aoe