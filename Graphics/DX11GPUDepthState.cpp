#include "pch.h"

#include "DX11GPUDepthState.h"
#include "DX11Helper.h"

namespace aoe {

void* DX11GPUDepthState::GetNative() const {
	return depth_stencil_state_.Get();
}

const GPUDepthStateDescription& DX11GPUDepthState::GetDescription() const {
	return description_;
}

DX11GPUDepthState::DX11GPUDepthState(const DX11GPUDevice& device, GPUDepthStateDescription description)
	: device_(device)
	, description_(std::move(description))
	, depth_stencil_state_(nullptr)
{
	D3D11_DEPTH_STENCIL_DESC depth_stencil_desc = {};
	depth_stencil_desc.DepthEnable = description_.is_depth_enabled;
	depth_stencil_desc.DepthWriteMask = ToDXDepthWirteMask(description_.write_mask);
	depth_stencil_desc.DepthFunc = ToDXComparsionFunc(description_.comparsion_function);

	const HRESULT hr = device_.GetNative()->CreateDepthStencilState(&depth_stencil_desc, &depth_stencil_state_);
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create depth state.");
}

D3D11_DEPTH_WRITE_MASK DX11GPUDepthState::ToDXDepthWirteMask(const GPUDepthWriteMask value) {
	return static_cast<D3D11_DEPTH_WRITE_MASK>(value);
}

D3D11_COMPARISON_FUNC DX11GPUDepthState::ToDXComparsionFunc(const GPUComparsionFunction value) {
	return static_cast<D3D11_COMPARISON_FUNC>(value);
}

} // namespace aoe