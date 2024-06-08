#pragma once

#include "Transform.h"

namespace aoe {

class TransformComponent {
public:
	Transform transform;

	TransformComponent()
		: transform()
	{}

	TransformComponent(Transform transform)
		: transform(transform)
	{}

	TransformComponent(Vector3f position, Quaternion rotation, Vector3f scale)
		: transform(position, rotation, scale)
	{}
};

} // namespace aoe