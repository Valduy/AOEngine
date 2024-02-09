#pragma once

#include "DX11GPUDevice.h"
#include "DX11GPUByteCode.h"

namespace aoe {

class DX11GPUPixelShader {
public:
	ID3D11PixelShader* GetNative() const;

	DX11GPUPixelShader(const DX11GPUByteCode& byte_code);

private:
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader_;
};

} // namespace aoe