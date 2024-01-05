#include "pch.h"

#include "GPUDepthState.h"

namespace aoe {

ID3D11DepthStencilState* GPUDepthState::GetNative() const {
	return depth_stencil_state_;
}

const GPUDepthStateDescription& GPUDepthState::GetDescription() const {
	return description_;
}

GPUDepthState::GPUDepthState(const GPUDevice& device)
	: depth_stencil_state_(nullptr)
	, device_(device)
{}

bool GPUDepthState::Initialize(const GPUDepthStateDescription& description) {
	AOE_ASSERT(depth_stencil_state_ == nullptr);
	description_ = description;

	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = description_.is_depth_enabled;
	depth_stencil_desc.DepthWriteMask = ToDXDepthWirteMask(description_.write_mask);
	depth_stencil_desc.DepthFunc = ToDXComparsionFunc(description_.comparsion_function);

	HRESULT hr = device_.GetNative()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_);
	return SUCCEEDED(hr);
}

void GPUDepthState::Terminate() {
	AOE_DX_SAFE_RELEASE(depth_stencil_state_);
}

D3D11_DEPTH_WRITE_MASK GPUDepthState::ToDXDepthWirteMask(const GPUDepthWriteMask value) {
	return static_cast<D3D11_DEPTH_WRITE_MASK>(value);
}

D3D11_COMPARISON_FUNC GPUDepthState::ToDXComparsionFunc(const GPUComparsionFunction value) {
	return static_cast<D3D11_COMPARISON_FUNC>(value);
}

} // namespace aoe