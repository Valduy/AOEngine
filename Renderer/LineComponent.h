#pragma once

#include <vector>

#include "../Core/Math.h"

#include "Colors.h"

namespace aoe {

struct LineComponent {
	Vector3f color;
	std::vector<Vector3f> points;

	LineComponent()
		: color(Colors::kWhite)
		, points()
	{}

	LineComponent(Vector3f color, std::vector<Vector3f> points)
		: color(color)
		, points(std::move(points))
	{}
};

} // namespace aoe