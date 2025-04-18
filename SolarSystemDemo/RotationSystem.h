#pragma once

#include "../Game/ECSSystemBase.h"
#include "../Game/TransformComponent.h"

#include "RotationComponent.h"

class RotationSystem : public aoe::ECSSystemBase {
public:
	void Update(float dt) override {
		using namespace aoe;

		for (Entity entity : FilterEntities<TransformComponent, RotationComponent>()) {
			auto transform_component = GetComponent<TransformComponent>(entity);
			auto rotation_component = GetComponent<RotationComponent>(entity);

			Quaternion rotator = Quaternion::FromAngleAxis(
				rotation_component->speed * Math::kDeg2Rad, 
				rotation_component->axis);
			
			Quaternion rotation = rotator * transform_component->GetRotation();
			transform_component->SetRotation(rotation);
		}
	}
};