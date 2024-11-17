#pragma once

#include "../Core/Math.h"

struct RotationComponent {
	aoe::Vector3f axis;
	float speed;

	RotationComponent()
		: axis(aoe::Math::kUp)
		, speed(0)
	{}

	RotationComponent(aoe::Vector3f axis, float speed)
		: axis(axis)
		, speed(speed)
	{}
};