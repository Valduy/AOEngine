#include "pch.h"
#include "GPUPixelShader.h"

namespace aoe {

ID3D11PixelShader* aoe::GPUPixelShader::GetNative() const {
	return pixel_shader_.Get();
}

GPUPixelShader::GPUPixelShader(const GPUDevice& device, const DXByteCode& byte_code)
	: device_(device)
	, pixel_shader_(nullptr)
{
	const HRESULT hr = device_.GetNative()->CreatePixelShader(
		byte_code.GetBufferPointer(),
		byte_code.GetBufferSize(),
		nullptr,
		pixel_shader_.GetAddressOf());

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create pixel shader.");
}

} // namespace aoe