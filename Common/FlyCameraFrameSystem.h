#pragma once

#include "../Game/ECSSystemBase.h"
#include "../Application/Application.h"
#include "../Renderer/CameraUtils.h"

#include "FlyCameraComponent.h"

namespace aoe {

class FlyCameraFrameSystem : public ECSSystemBase {
public:
	void Update(float dt) override;

private:
	Vector3f GetMovement(const Transform& transform, CH<FlyCameraComponent> fly_camera_component, float dt);
};

} // namespace aoe