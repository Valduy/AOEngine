#pragma once

#include "../Graphics/DX11GPUContext.h"
#include "../Graphics/DX11GPUBuffer.h"

namespace aoe {

template<typename TData>
struct DX11RenderData {
	DX11GPUBuffer buffer;

	DX11RenderData()
		: buffer(DX11GPUBuffer::Create<TData>({ GPUBufferType::kConstantBuffer, GPUResourceUsage::kDynamic }))
	{}

	void Update(const TData* data) {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.UpdateBuffer<TData>(buffer, data, 1);
	}
};

} // namespace aoe