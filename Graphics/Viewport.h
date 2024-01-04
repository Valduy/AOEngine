#pragma once

namespace aoe {

struct Viewport {
	float x;
	float y;
	float width;
	float height;
	float min_depth;
	float max_depth;

	Viewport()
		: x(0.0f)
		, y(0.0f)
		, width(0.0f)
		, height(0.0f)
		, min_depth(0.0f)
		, max_depth(1.0f)
	{}

	Viewport(float x, float y, float width, float height)
		: x(x)
		, y(y)
		, width(width)
		, height(height)
		, min_depth(0.0f)
		, max_depth(1.0f)
	{}

	Viewport(float x, float y, float width, float height, float min_depth, float max_depth)
		: x(x)
		, y(y)
		, width(width)
		, height(height)
		, min_depth(min_depth)
		, max_depth(max_depth)
	{}
};

} // namespace aoe