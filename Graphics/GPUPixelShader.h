#pragma once

#include "DX11GPUDevice.h"
#include "DXByteCode.h"

namespace aoe {

class GPUPixelShader {
public:
	ID3D11PixelShader* GetNative() const;

	GPUPixelShader(const DX11GPUDevice& device, const DXByteCode& byte_code);

private:
	const DX11GPUDevice& device_;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;
};

} // namespace aoe