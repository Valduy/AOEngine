#pragma once

#include "GPUDevice.h"
#include "IGPUResource.h"

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

class GPUSampler : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUSampler)

	ID3D11SamplerState* GetNative() const {
		return sampler_;
	}

	GPUSampler(const GPUDevice& device)
		: device_(device)
		, sampler_(nullptr)
	{}

	bool Initialize(const GPUSamplerDescription& description) 
	{
		description_ = description;

		D3D11_SAMPLER_DESC sampler_desc{};
		sampler_desc.Filter = ToDXFilter(description_.filter, description_.comparsion_function);
		sampler_desc.AddressU = ToDXAddressMode(description_.address_u);
		sampler_desc.AddressV = ToDXAddressMode(description_.address_v);
		sampler_desc.AddressW = ToDXAddressMode(description_.address_w);
		sampler_desc.MipLODBias = description_.mip_bias;
		sampler_desc.MaxAnisotropy = description_.max_anisotropy;
		sampler_desc.ComparisonFunc = ToDXComparsionFunction(description_.comparsion_function);
		FillBorderColor(description_.border_color, sampler_desc.BorderColor);
		sampler_desc.MinLOD = description_.min_mip_level;
		sampler_desc.MaxLOD = description_.max_mip_level;

		const HRESULT hr = device_.GetNative()->CreateSamplerState(&sampler_desc, &sampler_);
		AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);
	}

	void Terminate() override {
		AOE_DX_SAFE_RELEASE(sampler_);
	}

private:
	const GPUDevice& device_;
	GPUSamplerDescription description_;
	ID3D11SamplerState* sampler_;

	static D3D11_FILTER ToDXFilter(const GPUSamplerFilter filter, const GPUSamplerComparsionFunction comparsion_function) {
		switch (comparsion_function) {
		case GPUSamplerComparsionFunction::kNever:
			switch (filter) {
			case GPUSamplerFilter::kPoint:
				return D3D11_FILTER_MIN_MAG_MIP_POINT;
			case GPUSamplerFilter::kBilinear:
				return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
			case GPUSamplerFilter::kTrilinear:
				return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
			case GPUSamplerFilter::kAnisotropic:
				return D3D11_FILTER_ANISOTROPIC;
			default:
				AOE_ASSERT_MSG(false, "Unknown filter.");
				return D3D11_FILTER{};
			}
		case GPUSamplerComparsionFunction::kLess:
			switch (filter) {
			case GPUSamplerFilter::kPoint:
				return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
			case GPUSamplerFilter::kBilinear:
				return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
			case GPUSamplerFilter::kTrilinear:
				return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
			case GPUSamplerFilter::kAnisotropic:
				return D3D11_FILTER_COMPARISON_ANISOTROPIC;
			default:
				AOE_ASSERT_MSG(false, "Unknown filter.");
				return D3D11_FILTER{};
			}
		default:
			AOE_ASSERT_MSG(false, "Unknown comparsion function.");
			return D3D11_FILTER{};
		}
	}

	static D3D11_COMPARISON_FUNC ToDXComparsionFunction(const GPUSamplerComparsionFunction comparsion_function) {
		switch (comparsion_function) {
		case GPUSamplerComparsionFunction::kNever:
			return D3D11_COMPARISON_NEVER;
		case GPUSamplerComparsionFunction::kLess:
			return D3D11_COMPARISON_LESS;
		default:
			AOE_ASSERT_MSG(false, "Unknown comparsion function.");
			return D3D11_COMPARISON_FUNC{};
		}
	}

	static D3D11_TEXTURE_ADDRESS_MODE ToDXAddressMode(const GPUSamplerAddressMode address_mode) {
		switch (address_mode) {
		case GPUSamplerAddressMode::kWrap:
			return D3D11_TEXTURE_ADDRESS_WRAP;
		case GPUSamplerAddressMode::kClamp:
			return D3D11_TEXTURE_ADDRESS_CLAMP;
		case GPUSamplerAddressMode::kMirror:
			return D3D11_TEXTURE_ADDRESS_MIRROR;
		case GPUSamplerAddressMode::kBorder:
			return D3D11_TEXTURE_ADDRESS_BORDER;
		default:
			AOE_ASSERT_MSG(false, "Unknown texture address mode.");
			return D3D11_TEXTURE_ADDRESS_MODE{};
		}
	}

	static void FillBorderColor(const GPUSamplerBorderColor border_color, float color[4]) {
		switch (border_color) {
		case GPUSamplerBorderColor::kTransparentBlack:
			color[0] = 0.0f;
			color[1] = 0.0f;
			color[2] = 0.0f;
			color[3] = 0.0f;
			return;
		case GPUSamplerBorderColor::kOpaqueBlack:
			color[0] = 0.0f;
			color[1] = 0.0f;
			color[2] = 0.0f;
			color[3] = 1.0f;
			return;
		case GPUSamplerBorderColor::kOpaqueWhite:
			color[0] = 1.0f;
			color[1] = 1.0f;
			color[2] = 1.0f;
			color[3] = 1.0f;
			return;
		default:
			AOE_ASSERT_MSG(false, "Unknown sampler border color.");
		}
	}
};

} // namespace aoe