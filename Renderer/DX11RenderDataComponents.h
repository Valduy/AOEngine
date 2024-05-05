#pragma once

#include "../Core/Math.h"
#include "../Graphics/DX11GPUContext.h"
#include "../Graphics/DX11GPUBuffer.h"

namespace aoe {

struct TransformData {
	Matrix4 world;
	Matrix4 world_view_projection;
	Matrix4 inverse_transpose_world;
};

struct MaterialData {
	Vector3 diffuse;
	float dummy0;
	Vector3 specular;
	float shininess;
};

struct AmbientLightData {
	Vector3 color;
	float dummy0;
	float intensity;
	Vector3 dummy1;
};

struct DirectionalLightData {
	Vector3 view_position;
	float dummy0;
	Vector3 direction;
	float dummy1;
	Vector3 color;
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