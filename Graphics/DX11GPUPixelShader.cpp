#include "pch.h"

#include "DX11GPUPixelShader.h"
#include "DX11Helper.h"

namespace aoe {

ID3D11PixelShader* aoe::DX11GPUPixelShader::GetNative() const {
	return pixel_shader_.Get();
}

DX11GPUPixelShader::DX11GPUPixelShader(const DX11GPUByteCode& byte_code)
	: pixel_shader_(nullptr)
{
	const HRESULT hr = DX11GPUDevice::Instance()->GetNative()->CreatePixelShader(
		byte_code.GetBufferPointer(),
		byte_code.GetBufferSize(),
		nullptr,
		pixel_shader_.GetAddressOf());

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create pixel shader.");
}

} // namespace aoe