#pragma once

#include "Transform.h"

namespace aoe {

class TransformSystem;

class TransformComponent {
private:
	friend class TransformSystem;

public:
	TransformComponent()
		: TransformComponent(Transform())
	{}

	TransformComponent(Vector3f position, Quaternion rotation, Vector3f scale)
		: TransformComponent(Transform(position, rotation, scale))
	{}

	TransformComponent(Matrix4f& world_matrix)
		: TransformComponent(Transform(world_matrix))
	{}

	TransformComponent(Transform transform)
		: transform_(transform)
		, global_world_matrix_(Matrix4f::Identity())
		, has_changed_(true)
	{}

	const Transform& GetTransform() const {
		return transform_;
	}

	void SetTransform(const Transform& value) {
		transform_ = value;
		has_changed_ = true;
	}

	const Vector3f& GetPosition() const {
		return transform_.position;
	}

	void SetPosition(const Vector3f& value) {
		transform_.position = value;
		has_changed_ = true;
	}

	const Quaternion& GetRotation() const {
		return transform_.rotation;
	}

	void SetRotation(const Quaternion& value) {
		transform_.rotation = value;
		has_changed_ = true;
	}

	const Vector3f& GetScale() const {
		return transform_.scale;
	}

	void SetScale(const Vector3f& value) {
		transform_.scale = value;
		has_changed_ = true;
	}

	Matrix4f GetWorldMatrix() const {
		return transform_.ToMatrix();
	}

	void SetWorldMatrix(const Matrix4f& value) {
		transform_ = value;
		has_changed_ = true;
	}

	bool HasChanged() const {
		return has_changed_;
	}

	const Matrix4f& GetGlobalWorldMatrix() const {
		return global_world_matrix_;
	}

private:
	Transform transform_;
	Matrix4f global_world_matrix_;
	bool has_changed_;

	void SetGlobalWorldMatrix(const Matrix4f& value) {
		global_world_matrix_ = value;
		has_changed_ = false;
	}
};

} // namespace aoe