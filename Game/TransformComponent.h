#pragma once

#include "../Core/Math.h"

namespace aoe {

class TransformComponent {
public:
	Vector3 position;
	Quaternion rotation;
	Vector3 scale;

	TransformComponent()
		: position(Math::kV3Zero)
		, rotation(Math::kQIdentity)
		, scale(Math::kV3Ones)
	{}

	Matrix4 GetWorldMatrix() const {
		Matrix4 world_matrix = Matrix4::Identity();
		world_matrix *= Matrix4::FromTranslationVector(position);
		world_matrix *= rotation.ToMatrix4();
		world_matrix *= Matrix4::FromScaleVector(scale);
		return world_matrix;
	}

	Vector3 GetRight() const {
		Matrix4 world_matrix = GetWorldMatrix();
		return world_matrix * Math::kRight;
	}

	Vector3 GetLeft() const {
		Matrix4 world_matrix = GetWorldMatrix();
		return world_matrix * Math::kLeft;
	}

	Vector3 GetUp() const {
		Matrix4 world_matrix = GetWorldMatrix();
		return world_matrix * Math::kUp;
	}

	Vector3 GetDown() const {
		Matrix4 world_matrix = GetWorldMatrix();
		return world_matrix * Math::kDown;
	}

	Vector3 GetForward() const {
		Matrix4 world_matrix = GetWorldMatrix();
		return world_matrix * Math::kForward;
	}

	Vector3 GetBackward() const {
		Matrix4 world_matrix = GetWorldMatrix();
		return world_matrix * Math::kBackward;
	}
};

} // namespace aoe