#pragma once

#include "GPUDevice.h"
#include "DXByteCode.h"
#include "GPULayoutDescriptor.h"
#include "GPUResourceWrapper.h"

namespace aoe {

using GPUInputLayout = GPUResourceWrapper<ID3D11InputLayout>;

class GPUVertexShader {
public:
	ID3D11VertexShader* GetNative() const;
	GPUInputLayout GetInputLayout() const;

	GPUVertexShader(const GPUDevice& device, const DXByteCode& byte_code, const GPULayoutDescriptor& layout_desc);

private:
	const GPUDevice& device_;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader_;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout_;
};

} // namespace aoe