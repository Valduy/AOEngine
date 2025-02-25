#pragma once

#include "../Core/Math.h"

#include "DX11RenderData.h"
#include "DX11BufferModels.h"

namespace aoe {

enum class Projection {
	kPerspective,
	kOrthographic,
};

class DX11PreRenderPassSystem;

class DX11CameraComponent {
private:
	friend class DX11PreRenderPassSystem;

public:
	Projection projection;
	float width;
	float height;
	float far_plain;
	float near_plain;
	float field_of_view;

	Matrix4f GetProjectionMatrix() {
		switch (projection) {
		case Projection::kPerspective:
			return Matrix4f::Perspective(field_of_view, width / height, near_plain, far_plain, Math::kLH);
		case Projection::kOrthographic:
			return Matrix4f::Ortho(0, width, height, 0, near_plain, far_plain, Math::kLH);
		default:
			AOE_ASSERT_MSG(false, "Not supported projection.");
		}

		return Matrix4f::Identity();
	}

	const DX11RenderData<Matrix4f>& GetCameraData() const {
		return camera_data;
	}

	const DX11RenderData<Vector3fData>& GetTransformData() const {
		return transform_data;
	}

private:
	DX11RenderData<Matrix4f> camera_data;
	DX11RenderData<Vector3fData> transform_data;
};

} // namespace aoe