#pragma once

#include "../Core/Math.h"

namespace aoe {

struct Vertex {
	const Vector3f position;
	const Vector3f normal;
	const Vector2f uv;

	Vertex(Vector3f position, Vector3f normal, Vector2f uv)
		: position(position)
		, normal(normal)
		, uv(uv)
	{}

	Vertex()
		: position(Math::kZeros3f)
		, normal(Math::kZeros3f)
		, uv(Math::kZeros2f)
	{}
};

} // namespace aoe