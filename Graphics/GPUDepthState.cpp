#include "pch.h"

#include "GPUDepthState.h"

namespace aoe {

ID3D11DepthStencilState* GPUDepthState::GetNative() const {
	return depth_stencil_state_;
}

GPUDepthState::GPUDepthState(const GPUDevice& device)
	: depth_stencil_state_(nullptr)
	, device_(device)
{}

bool GPUDepthState::Initialize(bool is_depth_enabled, GPUDepthWriteMask write_mask, GPUComparsionFunc comparsion_func) {
	AOE_ASSERT(depth_stencil_state_ == nullptr);

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = is_depth_enabled;
	depth_stencil_desc.DepthWriteMask = DXHelper::ToDepthWirteMask(write_mask);
	depth_stencil_desc.DepthFunc = DXHelper::ToComparsionFunc(comparsion_func);

	HRESULT hr = device_.GetNative()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_);
	return SUCCEEDED(hr);
}

void GPUDepthState::Terminate() {
	AOE_DX_SAFE_RELEASE(depth_stencil_state_);
}

} // namespace aoe