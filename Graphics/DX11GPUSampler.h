#pragma once

#include "GPUEnums.h"
#include "DX11GPUDevice.h"

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

class DX11GPUSampler {
public:
	DX11GPUSampler(const GPUSamplerDescription& description);

	ID3D11SamplerState* GetNative() const;
	const GPUSamplerDescription& GetDescription() const;

private:
	GPUSamplerDescription description_;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_;

	static D3D11_FILTER ToDXFilter(const GPUSamplerFilter filter, const GPUSamplerComparsionFunction comparsion_function);
	static D3D11_COMPARISON_FUNC ToDXComparsionFunction(const GPUSamplerComparsionFunction comparsion_function);
	static D3D11_TEXTURE_ADDRESS_MODE ToDXAddressMode(const GPUSamplerAddressMode address_mode);
	static void FillBorderColor(const GPUSamplerBorderColor border_color, float color[4]);
};

} // namespace aoe