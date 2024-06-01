#pragma once

#include "../Game/IECSSystem.h"
#include "../Game/ServiceProvider.h"
#include "../Application/Application.h"
#include "../ECS/World.h"
#include "../Renderer/CameraUtils.h"

#include "FlyCameraComponent.h"

namespace aoe {

class FlyCameraSystem : public IECSSystem {
public:
	FlyCameraSystem(const aoe::ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void PerTickUpdate(float dt) override;
	void PerFrameUpdate(float dt) override;

private:
	static constexpr float kEpsilon = 0.001f;

	const ServiceProvider& service_provider_;

	Application* application_;
	World* world_;

	bool is_forward_;
	bool is_backward_;
	bool is_left_;
	bool is_right_;
	bool is_up_;
	bool is_down_;
	
	void CaptureInputs();
	Vector3f GetMovement(ComponentHandler<TransformComponent> transform_component, float speed, float dt);
};

} // namespace aoe