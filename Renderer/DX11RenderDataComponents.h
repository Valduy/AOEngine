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

struct PointLightData {
	Vector3f view_position;
	float dummy0;
	Vector3f position;
	float dummy1;
	Vector3f color;
	float dummy2;
};

struct LineData {
	Matrix4f world_view_projection;
	Vector3f color;
	float dummy;
};

template<typename TData>
struct DX11RenderData {
	DX11GPUBuffer buffer;

	DX11RenderData()
		: buffer(DX11GPUBuffer::Create<TData>({GPUBufferType::kConstantBuffer, GPUResourceUsage::kDynamic}))
	{}

	void Update(TData* data) {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.UpdateBuffer<TData>(buffer, data, 1);
	}
};

struct GeometryDataComponent {
	DX11RenderData<TransformData> transform_data;
	DX11RenderData<MaterialData> material_data;
};

struct AmbientLightDataComponent {
	DX11RenderData<AmbientLightData> light_data;
};

struct DirectionalLightDataComponent {
	DX11RenderData<DirectionalLightData> light_data;
};

struct PointLightDataComponent {
	DX11RenderData<Matrix4f> transform_data;
	DX11RenderData<PointLightData> light_data;
};

struct LineDataComponent {
	DX11RenderData<LineData> line_data;
};

} // namespace aoe