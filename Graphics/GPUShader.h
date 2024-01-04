#pragma once

#include "GPUDevice.h"
#include "DXByteCode.h"
#include "GPULayoutDescriptor.h"
#include "GPUResourceWrapper.h"

namespace aoe {

using GPUInputLayout = GPUResourceWrapper<ID3D11InputLayout>;

class GPUVertexShader : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUVertexShader)

	ID3D11VertexShader* GetNative() const {
		return vertex_shader_;
	}

	GPUInputLayout GetInputLayout() const {
		AOE_ASSERT(input_layout_);
		return { input_layout_ };
	}

	GPUVertexShader(const GPUDevice& device)
		: vertex_shader_(nullptr)
		, input_layout_(nullptr)
		, device_(device)
	{}

	bool Initialize(const DXByteCode& byte_code, const GPULayoutDescriptor& layout_desc) {
		HRESULT hr = S_OK;
		hr = device_.GetNative()->CreateVertexShader(
			byte_code.GetBufferPointer(),
			byte_code.GetBufferSize(),
			nullptr,
			&vertex_shader_);

		AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);

		hr = device_.GetNative()->CreateInputLayout(
			layout_desc.GetBufferPointer(),
			layout_desc.GetBufferSize(),
			byte_code.GetBufferPointer(),
			byte_code.GetBufferSize(),
			&input_layout_);

		AOE_DX_TERMINATE_AND_RETURN_ON_FAILURE(hr, false);

		return true;
	}

	void Terminate() {
		AOE_DX_SAFE_RELEASE(input_layout_);
		AOE_DX_SAFE_RELEASE(vertex_shader_);
	}

private:
	ID3D11VertexShader* vertex_shader_;
	ID3D11InputLayout* input_layout_;

	const GPUDevice& device_;
};

class GPUPixelShader : public IGPUResource {
public:
	AOE_NON_COPYABLE_CLASS(GPUPixelShader)

	ID3D11PixelShader* GetNative() const {
		return pixel_shader_;
	}

	GPUPixelShader(const GPUDevice& device)
		: pixel_shader_(nullptr)
		, device_(device)
	{}

	bool Initialize(const DXByteCode& byte_code) {
		HRESULT hr = device_.GetNative()->CreatePixelShader(
			byte_code.GetBufferPointer(),
			byte_code.GetBufferSize(),
			nullptr,
			&pixel_shader_);

		return SUCCEEDED(hr);
	}

	void Terminate() {
		AOE_DX_SAFE_RELEASE(pixel_shader_);
	}

private:
	ID3D11PixelShader* pixel_shader_;

	const GPUDevice& device_;
};

} // namespace aoe