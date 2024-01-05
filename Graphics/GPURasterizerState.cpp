#include "pch.h"

#include "../Core/Debug.h"

#include "GPURasterizerState.h"
#include "DXHelper.h"

namespace aoe {

ID3D11RasterizerState* GPURasterizerState::GetNative() const {
	return rasterizer_state_;
}

const GPURasteriserStateDescription& GPURasterizerState::GetDescription() const {
	return description_;
}

GPURasterizerState::GPURasterizerState(const GPUDevice& device)
	: rasterizer_state_(nullptr)
	, device_(device)
{}

bool GPURasterizerState::Initialize(const GPURasteriserStateDescription& description) {
	AOE_ASSERT(rasterizer_state_ == nullptr);
	description_ = description;

	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.CullMode = ToDXCullMode(description_.cull_mode);
	rasterizer_desc.FillMode = ToDXFillMode(description_.fill_mode);

	HRESULT hr = device_.GetNative()->CreateRasterizerState(&rasterizer_desc, &rasterizer_state_);
	return SUCCEEDED(hr);
}

void GPURasterizerState::Terminate() {
	AOE_DX_SAFE_RELEASE(rasterizer_state_);
}

D3D11_CULL_MODE GPURasterizerState::ToDXCullMode(const GPUCullMode value) {
	return static_cast<D3D11_CULL_MODE>(value);
}

D3D11_FILL_MODE GPURasterizerState::ToDXFillMode(const GPUFillMode value) {
	return static_cast<D3D11_FILL_MODE>(value);
}

} // namespace aoe