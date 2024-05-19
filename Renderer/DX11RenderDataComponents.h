#pragma once

#include "../Core/Math.h"
#include "../Graphics/DX11GPUContext.h"
#include "../Graphics/DX11GPUBuffer.h"

namespace aoe {

struct TransformData {
	Matrix4f world;
	Matrix4f world_view_projection;
	Matrix4f inverse_transpose_world;
};

struct MaterialData {
	Vector3f diffuse;
	float dummy0;
	Vector3f specular;
	float shininess;
};

struct AmbientLightData {
	Vector3f color;
	float dummy0;
	float intensity;
	Vector3f dummy1;
};

struct DirectionalLightData {
	Vector3f view_position;
	float dummy0;
	Vector3f direction;
	float dummy1;
	Vector3f color;
	float dummy2;
};

template<typename TData>
class DX11RenderDataComponent {
public:
	DX11GPUBuffer buffer;

	DX11RenderDataComponent()
		: buffer(DX11GPUBuffer::Create<TData>({GPUBufferType::kConstantBuffer, GPUResourceUsage::kDynamic}))
	{}

	void Update(TData* data) {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.UpdateBuffer<TData>(buffer, data, 1);
	}
};

using DX11TransformDataComponent = DX11RenderDataComponent<TransformData>;
using DX11MaterialDataComponent = DX11RenderDataComponent<MaterialData>;
using DX11AmbientLightDataComponent = DX11RenderDataComponent<AmbientLightData>;
using DX11DirectionalLightDataComponent = DX11RenderDataComponent<DirectionalLightData>;

} // namespace aoe