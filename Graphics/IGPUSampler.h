#pragma once

#include "GPUEnums.h"

namespace aoe {

struct GPUSamplerDescription {
	GPUSamplerFilter filter;
	GPUSamplerAddressMode address_u;
	GPUSamplerAddressMode address_v;
	GPUSamplerAddressMode address_w;
	GPUSamplerComparsionFunction comparsion_function;
	GPUSamplerBorderColor border_color;
	float mip_bias;
	float min_mip_level;
	float max_mip_level;
	uint32_t max_anisotropy;

	GPUSamplerDescription(
		GPUSamplerFilter filter,
		GPUSamplerAddressMode address_u,
		GPUSamplerAddressMode address_v,
		GPUSamplerAddressMode address_w,
		GPUSamplerComparsionFunction comparsion_function,
		GPUSamplerBorderColor border_color,
		float mip_bias,
		float min_mip_level,
		float max_mip_level,
		uint32_t max_anisotropy
	)
		: filter(filter)
		, address_u(address_u)
		, address_v(address_v)
		, address_w(address_w)
		, comparsion_function(comparsion_function)
		, border_color(border_color)
		, mip_bias(mip_bias)
		, min_mip_level(min_mip_level)
		, max_mip_level(max_mip_level)
		, max_anisotropy(max_anisotropy)
	{}

	GPUSamplerDescription()
		: filter(GPUSamplerFilter::kPoint)
		, address_u(GPUSamplerAddressMode::kWrap)
		, address_v(GPUSamplerAddressMode::kWrap)
		, address_w(GPUSamplerAddressMode::kWrap)
		, comparsion_function(GPUSamplerComparsionFunction::kNever)
		, border_color(GPUSamplerBorderColor::kOpaqueBlack)
		, mip_bias(0)
		, min_mip_level(0)
		, max_mip_level(0)
		, max_anisotropy(0)
	{}
};

class IGPUSampler {
public:
	virtual void* GetNative() const = 0;
	virtual const GPUSamplerDescription& GetDescription() const = 0;

	virtual ~IGPUSampler() = default;
};

} // namespace aoe