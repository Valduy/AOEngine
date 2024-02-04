#pragma once

#include "IGPUTextureView.h"

namespace aoe {

class IGPUSwapChain {
public:
public:
	virtual void* GetNative() const = 0;
	virtual const IGPUTextureView* GetRenderTargetView() const = 0;

	virtual bool Resize(size_t width, size_t height) = 0;
	virtual void Present() = 0;

	virtual ~IGPUSwapChain() = default;
};

} // namespace aoe