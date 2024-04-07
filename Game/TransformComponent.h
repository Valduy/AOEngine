#pragma once

#include "../Core/Math.h"

namespace aoe {

class TransformComponent {
public:
	aoe::Vector3 position;
	aoe::Quaternion rotation;
	aoe::Vector3 scale;

	TransformComponent()
		: position(aoe::Math::kV3Zero)
		, rotation(aoe::Math::kQIdentity)
		, scale(aoe::Math::kV3Ones)
	{}

	aoe::Matrix4 GetWorldMatrix() const {
		aoe::Matrix4 model = aoe::Matrix4::Identity();
		model *= aoe::Matrix4::FromTranslationVector(position);
		model *= rotation.ToMatrix4();
		model *= aoe::Matrix4::FromScaleVector(scale);
		return model;
	}
};

} // namespace aoe