#pragma once

#include "../Core/Math.h"

#include "DX11RenderData.h"

namespace aoe {

struct TransformData {
	Matrix4f world;
	Matrix4f world_it;
};

struct MaterialData {
	Vector3f diffuse;
	float dummy;
	Vector3f specular;
	float shininess;
};

struct PointLightCameraData {
	Matrix4f view_projection;
	Vector3f view_position;
	float dummy;
};

struct PointLightTransformData {
	Matrix4f world;
	Vector3f position;
	float dummy;
};

struct Vector3fData {
	Vector3f value;
	float dummy;
};

} // namespace aoe