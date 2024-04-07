#pragma once

#include "../Core/Math.h"
#include "../Graphics/DX11GPUBuffer.h"

namespace aoe {

struct TransformData {
	Matrix4 world;
	Matrix4 world_view_projection;
	Matrix4 inverse_transpose_world;
};

class DX11RenderDataComponent {
public:
	DX11GPUBuffer transform_data_buffer;

	DX11RenderDataComponent()
		: transform_data_buffer(DX11GPUBuffer::Create<TransformData>(CreateBufferDescription()))
	{}

private:
	static GPUBufferDescription CreateBufferDescription() {
		return { GPUBufferType::kConstantBuffer, GPUResourceUsage::kDynamic };
	}
};

} // namespace aoe