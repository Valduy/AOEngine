#pragma once

#include "../Application/Window.h"

#include "IGPUSwapChain.h"
#include "IGPUBuffer.h"
#include "IGPUTexture2D.h"
#include "IGPUDepthState.h"
#include "IGPURasterizerState.h"
#include "IGPUSampler.h"

namespace aoe {

class IGPUDevice {
public:
	template<typename TElement>
	IGPUBuffer* CreateBuffer(const GPUBufferDescription& description, const TElement* data, size_t count) const;
	template<typename TElement>
	IGPUBuffer* CreateBuffer(const GPUBufferDescription& description, size_t count = 1) const;

	template<typename TElement>
	IGPUTexture2D* CreateTexture2D(const GPUTexture2DDescription& description, const TElement* data) const;

	virtual IGPUSwapChain* CreateSwapChain(const Window& window) const = 0;
	virtual IGPUBuffer* CreateBuffer(const GPUBufferDescription& description, const void* data, size_t size, size_t stride) const = 0;
	virtual IGPUTexture2D* CreateTexture2D(const GPUTexture2DDescription& description, const void* data = nullptr, size_t stride = 0) const = 0;
	virtual IGPUDepthState* CreateDepthState(const GPUDepthStateDescription& description) const = 0;
	virtual IGPURasterizerState* CreateRasterizerState(const GPURasterizerStateDescription& description) const = 0;
	virtual IGPUSampler* CreateSampler(const GPUSamplerDescription& description) const = 0;

	virtual ~IGPUDevice() = default;
};

template<typename TElement>
IGPUBuffer* IGPUDevice::CreateBuffer(const GPUBufferDescription& description, const TElement* data, size_t count) const {
	return CreateBuffer(description, data, sizeof(TElement) * count, sizeof(TElement));
}

template<typename TElement>
IGPUBuffer* IGPUDevice::CreateBuffer(const GPUBufferDescription& description, size_t count) const {
	return CreateBuffer(description, nullptr, sizeof(TElement) * count, sizeof(TElement));
}

template<typename TElement>
IGPUTexture2D* IGPUDevice::CreateTexture2D(const GPUTexture2DDescription& description, const TElement* data) const {
	return CreateTexture2D(description, data, sizeof(TElement));
}

}