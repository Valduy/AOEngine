#pragma once

#include "../Core/Math.h"

namespace aoe {

struct Vertex {
	const Vector3 position;
	const Vector3 normal;
	const Vector2 uv;

	Vertex(Vector3 position, Vector3 normal, Vector2 uv)
		: position(position)
		, normal(normal)
		, uv(uv)
	{}

	Vertex()
		: position(Math::kV3Zero)
		, normal(Math::kV3Zero)
		, uv(Math::kV2Zero)
	{}
};

} // namespace aoe