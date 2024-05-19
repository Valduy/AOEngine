#pragma once

#include "../Core/Math.h"

namespace aoe {

class TransformComponent {
public:
	Vector3f position;
	Quaternion rotation;
	Vector3f scale;

	TransformComponent()
		: position(Math::kZeros3f)
		, rotation(Math::kQIdentity)
		, scale(Math::kOnes3f)
	{}

	Matrix4f GetWorldMatrix() const {
		Matrix4f world_matrix = Matrix4f::Identity();
		world_matrix *= Matrix4f::FromTranslationVector(position);
		world_matrix *= rotation.ToMatrix4();
		world_matrix *= Matrix4f::FromScaleVector(scale);
		return world_matrix;
	}

	Vector3f GetRight() const {
		Matrix4f world_matrix = GetWorldMatrix();
		return world_matrix * Math::kRight;
	}

	Vector3f GetLeft() const {
		Matrix4f world_matrix = GetWorldMatrix();
		return world_matrix * Math::kLeft;
	}

	Vector3f GetUp() const {
		Matrix4f world_matrix = GetWorldMatrix();
		return world_matrix * Math::kUp;
	}

	Vector3f GetDown() const {
		Matrix4f world_matrix = GetWorldMatrix();
		return world_matrix * Math::kDown;
	}

	Vector3f GetForward() const {
		Matrix4f world_matrix = GetWorldMatrix();
		return world_matrix * Math::kForward;
	}

	Vector3f GetBackward() const {
		Matrix4f world_matrix = GetWorldMatrix();
		return world_matrix * Math::kBackward;
	}
};

} // namespace aoe