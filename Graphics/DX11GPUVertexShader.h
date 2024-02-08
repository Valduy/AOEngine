#pragma once

#include "DX11GPUDevice.h"
#include "DX11GPUByteCode.h"

namespace aoe {

class DX11GPUVertexShader {
public:
	ID3D11VertexShader* GetNative() const;
	ID3D11InputLayout* GetInputLayout() const;

	DX11GPUVertexShader(const DX11GPUDevice& device, const DX11GPUByteCode& byte_code);

private:
	const DX11GPUDevice& device_;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;

	static std::vector<D3D11_INPUT_ELEMENT_DESC> CreateInputLayoutDescriptor(ID3DBlob* byte_code);
	static D3D11_INPUT_ELEMENT_DESC ToInputParameterDesc(const D3D11_SIGNATURE_PARAMETER_DESC& signature_desc);
	static DXGI_FORMAT DeduceDxgiFormat(D3D_REGISTER_COMPONENT_TYPE component_type, BYTE mask);
};

} // namespace aoe