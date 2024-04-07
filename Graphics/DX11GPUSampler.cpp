#include "pch.h"

#include "../Core/Debug.h"

#include "DX11GPUSampler.h"
#include "DX11Helper.h"

namespace aoe {

DX11GPUSampler::DX11GPUSampler(const GPUSamplerDescription& description)
	: description_(description)
	, sampler_(nullptr)
{
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

	const HRESULT hr = DX11GPUDevice::Instance().GetNative()->CreateSamplerState(&sampler_desc, sampler_.GetAddressOf());
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create sampler state.");
}

ID3D11SamplerState* DX11GPUSampler::GetNative() const {
	return sampler_.Get();
}

const GPUSamplerDescription& DX11GPUSampler::GetDescription() const {
	return description_;
}

D3D11_FILTER DX11GPUSampler::ToDXFilter(const GPUSamplerFilter filter, const GPUSamplerComparsionFunction comparsion_function) {
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

D3D11_COMPARISON_FUNC DX11GPUSampler::ToDXComparsionFunction(const GPUSamplerComparsionFunction comparsion_function) {
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

D3D11_TEXTURE_ADDRESS_MODE DX11GPUSampler::ToDXAddressMode(const GPUSamplerAddressMode address_mode) {
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

void DX11GPUSampler::FillBorderColor(const GPUSamplerBorderColor border_color, float color[4]) {
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

} // namespace aoe

