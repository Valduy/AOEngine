#pragma once

#include "Transform.h"

namespace aoe {

class TransformComponent {
public:
	TransformComponent()
		: transform()
		, world_matrix(Matrix4f::Identity())
	{}

	TransformComponent(Transform transform)
		: transform(transform)
		, world_matrix(transform.ToMatrix())
	{}

	TransformComponent(Vector3f position, Quaternion rotation, Vector3f scale)
		: transform(position, rotation, scale)
		, world_matrix(transform.ToMatrix())
	{}

	const Transform& GetTransform() const {
		return transform;
	}

	void SetTransform(const Transform& value) {
		transform = value;
		world_matrix = transform.ToMatrix();
	}

	const Vector3f& GetPosition() const {
		return transform.position;
	}

	void SetPosition(const Vector3f& value) {
		transform.position = value;
		world_matrix = transform.ToMatrix();
	}

	const Quaternion& GetRotation() const {
		return transform.rotation;
	}

	void SetRotation(const Quaternion& value) {
		transform.rotation = value;
		world_matrix = transform.ToMatrix();
	}

	const Vector3f& GetScale() const {
		return transform.scale;
	}

	void SetScale(const Vector3f& value) {
		transform.scale = value;
		world_matrix = transform.ToMatrix();
	}

	const Matrix4f& GetWorldMatrix() const {
		return world_matrix;
	}

	void SetWorldMatrix(const Matrix4f& value) {
		transform = value;
		world_matrix = value;
	}

private:
	Transform transform;
	Matrix4f world_matrix;
};

} // namespace aoe