#pragma once

#include "GPUEnums.h"

namespace aoe {

struct GPUDepthStateDescription {
	bool is_depth_enabled;
	GPUDepthWriteMask write_mask;
	GPUComparsionFunction comparsion_function;

	GPUDepthStateDescription(
		bool is_depth_enabled,
		GPUDepthWriteMask write_mask,
		GPUComparsionFunction comparsion_function
	)
		: is_depth_enabled(is_depth_enabled)
		, write_mask(write_mask)
		, comparsion_function(comparsion_function)
	{}

	GPUDepthStateDescription()
		: is_depth_enabled(false)
		, write_mask(GPUDepthWriteMask::kWriteNone)
		, comparsion_function(GPUComparsionFunction::kNever)
	{}
};

class IGPUDepthState {
public:
	virtual void* GetNative() const = 0;
	virtual const GPUDepthStateDescription& GetDescription() const = 0;

	virtual ~IGPUDepthState() = default;
};

} // namespace aoe