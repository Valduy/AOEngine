#pragma once

#include "GPUEnums.h"

namespace aoe {

struct GPURasterizerStateDescription {
	GPUCullMode cull_mode;
	GPUFillMode fill_mode;

	GPURasterizerStateDescription(GPUCullMode cull_mode, GPUFillMode fill_mode)
		: cull_mode(cull_mode)
		, fill_mode(fill_mode)
	{}

	GPURasterizerStateDescription()
		: cull_mode(GPUCullMode::kBack)
		, fill_mode(GPUFillMode::kSolid)
	{}
};

class IGPURasterizerState {
public:
	virtual void* GetNative() const = 0;
	virtual const GPURasterizerStateDescription& GetDescription() const = 0;

	virtual ~IGPURasterizerState() = default;
};

} // namespace aoe