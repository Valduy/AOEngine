#include "pch.h"

#include "DX11GPUVertexShader.h"
#include "DX11Helper.h"

namespace aoe {

DX11GPUVertexShader::DX11GPUVertexShader(const DX11GPUByteCode& byte_code)
    : vertex_shader_(nullptr)
    , input_layout_(nullptr)
{
    HRESULT hr = S_OK;

    hr = DX11GPUDevice::Instance().GetNative()->CreateVertexShader(
        byte_code.GetBufferPointer(),
        byte_code.GetBufferSize(),
        nullptr,
        vertex_shader_.GetAddressOf());

    AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create vertex shader.");

    std::vector<D3D11_INPUT_ELEMENT_DESC> layout_desc = CreateInputLayoutDescriptor(byte_code.GetNative());

    hr = DX11GPUDevice::Instance().GetNative()->CreateInputLayout(
        layout_desc.data(),
        static_cast<uint32_t>(layout_desc.size()),
        byte_code.GetBufferPointer(),
        byte_code.GetBufferSize(),
        input_layout_.GetAddressOf());

    AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create input layout.");
}

ID3D11VertexShader* aoe::DX11GPUVertexShader::GetNative() const {
	return vertex_shader_.Get();
}

ID3D11InputLayout* DX11GPUVertexShader::GetInputLayout() const {
	return input_layout_.Get();
}

std::vector<D3D11_INPUT_ELEMENT_DESC> DX11GPUVertexShader::CreateInputLayoutDescriptor(ID3DBlob* byte_code)
{
    Microsoft::WRL::ComPtr<ID3D11ShaderReflection> shader_reflection = nullptr;
    HRESULT hr = D3DReflect(
        byte_code->GetBufferPointer(),
        byte_code->GetBufferSize(),
        IID_ID3D11ShaderReflection,
        reinterpret_cast<void**>(shader_reflection.GetAddressOf()));

    AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to reflect shader.");

    D3D11_SHADER_DESC shader_desc;
    shader_reflection->GetDesc(&shader_desc);

    std::vector<D3D11_INPUT_ELEMENT_DESC> input_layout_desc(shader_desc.InputParameters);

    for (uint32_t i = 0; i < shader_desc.InputParameters; ++i) {
        D3D11_SIGNATURE_PARAMETER_DESC param_desc;
        shader_reflection->GetInputParameterDesc(i, &param_desc);

        D3D11_INPUT_ELEMENT_DESC element_desc = ToInputParameterDesc(param_desc);
        input_layout_desc[i] = element_desc;
    }

    return input_layout_desc;
}

D3D11_INPUT_ELEMENT_DESC DX11GPUVertexShader::ToInputParameterDesc(const D3D11_SIGNATURE_PARAMETER_DESC& signature_desc) {
    D3D11_INPUT_ELEMENT_DESC element_desc;
    element_desc.SemanticName = signature_desc.SemanticName;
    element_desc.SemanticIndex = signature_desc.SemanticIndex;
    element_desc.Format = DeduceDxgiFormat(signature_desc.ComponentType, signature_desc.Mask);
    element_desc.InputSlot = 0;
    element_desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    element_desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    element_desc.InstanceDataStepRate = 0;

    return element_desc;
}

DXGI_FORMAT DX11GPUVertexShader::DeduceDxgiFormat(D3D_REGISTER_COMPONENT_TYPE component_type, BYTE mask) {
    DXGI_FORMAT format = DXGI_FORMAT::DXGI_FORMAT_UNKNOWN;

    if (mask == 1) {
        if (component_type == D3D_REGISTER_COMPONENT_UINT32) {
            format = DXGI_FORMAT_R32_UINT;
        }
        else if (component_type == D3D_REGISTER_COMPONENT_SINT32) {
            format = DXGI_FORMAT_R32_SINT;
        }
        else if (component_type == D3D_REGISTER_COMPONENT_FLOAT32) {
            format = DXGI_FORMAT_R32_FLOAT;
        }
    } else if (mask <= 3) {
        if (component_type == D3D_REGISTER_COMPONENT_UINT32) {
            format = DXGI_FORMAT_R32G32_UINT;
        } else if (component_type == D3D_REGISTER_COMPONENT_SINT32) {
            format = DXGI_FORMAT_R32G32_SINT;
        } else if (component_type == D3D_REGISTER_COMPONENT_FLOAT32) {
            format = DXGI_FORMAT_R32G32_FLOAT;
        }
    } else if (mask <= 7) {
        if (component_type == D3D_REGISTER_COMPONENT_UINT32) {
            format = DXGI_FORMAT_R32G32B32_UINT;
        } else if (component_type == D3D_REGISTER_COMPONENT_SINT32) {
            format = DXGI_FORMAT_R32G32B32_SINT;
        } else if (component_type == D3D_REGISTER_COMPONENT_FLOAT32) {
            format = DXGI_FORMAT_R32G32B32_FLOAT;
        }
    } else if (mask <= 15) {
        if (component_type == D3D_REGISTER_COMPONENT_UINT32) {
            format = DXGI_FORMAT_R32G32B32A32_UINT;
        } else if (component_type == D3D_REGISTER_COMPONENT_SINT32) {
            format = DXGI_FORMAT_R32G32B32A32_SINT;
        } else if (component_type == D3D_REGISTER_COMPONENT_FLOAT32) {
            format = DXGI_FORMAT_R32G32B32A32_FLOAT;
        }
    }

    return format;
}

} // namespace aoe