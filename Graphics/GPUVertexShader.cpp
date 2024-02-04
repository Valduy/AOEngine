#include "pch.h"
#include "GPUVertexShader.h"

namespace aoe {

ID3D11VertexShader* aoe::GPUVertexShader::GetNative() const {
	return vertex_shader_.Get();
}

GPUInputLayout GPUVertexShader::GetInputLayout() const {
	return { input_layout_.Get() };
}

GPUVertexShader::GPUVertexShader(const DX11GPUDevice& device, const DXByteCode& byte_code, const GPULayoutDescriptor& layout_desc)
	: device_(device)
	, vertex_shader_(nullptr)
	, input_layout_(nullptr)
{
	HRESULT hr = S_OK;

	hr = device_.GetNative()->CreateVertexShader(
		byte_code.GetBufferPointer(),
		byte_code.GetBufferSize(),
		nullptr,
		vertex_shader_.GetAddressOf());

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create vertex shader.");

	hr = device_.GetNative()->CreateInputLayout(
		layout_desc.GetBufferPointer(),
		layout_desc.GetBufferSize(),
		byte_code.GetBufferPointer(),
		byte_code.GetBufferSize(),
		input_layout_.GetAddressOf());

	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create input layout.");
}

} // namespace aoe