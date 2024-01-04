#include "pch.h"

#include "../Core/Debug.h"

#include "GPURasterizerState.h"
#include "DXHelper.h"

namespace aoe {

ID3D11RasterizerState* GPURasterizerState::GetNative() const {
	return rasterizer_state_;
}

GPURasterizerState::GPURasterizerState(const GPUDevice& device)
	: rasterizer_state_(nullptr)
	, device_(device)
{}

bool GPURasterizerState::Initialize(GPUCullMode cull_mode, GPUFillMode fill_mode) {
	AOE_ASSERT(rasterizer_state_ == nullptr);

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.CullMode = DXHelper::ToCullMode(cull_mode);
	rasterizer_desc.FillMode = DXHelper::ToFillMode(fill_mode);

	HRESULT hr = device_.GetNative()->CreateRasterizerState(&rasterizer_desc, &rasterizer_state_);
	return SUCCEEDED(hr);
}

void GPURasterizerState::Terminate() {
	AOE_DX_SAFE_RELEASE(rasterizer_state_);
}

} // namespace aoe