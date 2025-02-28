#pragma once

#include "../Core/Math.h"

namespace aoe {

struct Transform {
	Vector3f position;
	Quaternion rotation;
	Vector3f scale;

	Transform()
		: position(Math::kZeros3f)
		, rotation(Math::kQIdentity)
		, scale(Math::kOnes3f)
	{}

	Transform(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale)
		: position(position)
		, rotation(rotation)
		, scale(scale)
	{}

	Transform(const Matrix4f& world) 
		: position(world.TranslationVector3D())
		, rotation(Quaternion::FromMatrix(world))
		, scale(world.ScaleVector3D())
	{}

	Matrix4f ToMatrix() const {
		Matrix4f world_matrix = Matrix4f::Identity();
		world_matrix *= Matrix4f::FromTranslationVector(position);
		world_matrix *= rotation.ToMatrix4();
		world_matrix *= Matrix4f::FromScaleVector(scale);
		return world_matrix;
	}

	Vector3f GetRight() const {
		return rotation * Math::kRight;
	}

	Vector3f GetLeft() const {
		return rotation * Math::kLeft;
	}

	Vector3f GetUp() const {
		return rotation * Math::kUp;
	}

	Vector3f GetDown() const {
		return rotation * Math::kDown;
	}

	Vector3f GetForward() const {
		return rotation * Math::kForward;
	}

	Vector3f GetBackward() const {
		return rotation * Math::kBackward;
	}
};

} // namespace aoe