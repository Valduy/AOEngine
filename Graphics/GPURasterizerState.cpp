#include "pch.h"

#include "../Core/Debug.h"

#include "GPURasterizerState.h"
#include "DXHelper.h"

namespace aoe {

ID3D11RasterizerState* GPURasterizerState::GetNative() const {
	return rasterizer_state_.Get();
}

const GPURasteriserStateDescription& GPURasterizerState::GetDescription() const {
	return description_;
}

GPURasterizerState::GPURasterizerState(const GPUDevice& device, const GPURasteriserStateDescription& description)
	: device_(device)
	, description_(description)
	, rasterizer_state_(nullptr)
{
	D3D11_RASTERIZER_DESC rasterizer_desc = {};
	rasterizer_desc.CullMode = ToDXCullMode(description_.cull_mode);
	rasterizer_desc.FillMode = ToDXFillMode(description_.fill_mode);

	const HRESULT hr = device_.GetNative()->CreateRasterizerState(&rasterizer_desc, rasterizer_state_.GetAddressOf());
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create rasterizer state.");
}

D3D11_CULL_MODE GPURasterizerState::ToDXCullMode(const GPUCullMode value) {
	return static_cast<D3D11_CULL_MODE>(value);
}

D3D11_FILL_MODE GPURasterizerState::ToDXFillMode(const GPUFillMode value) {
	return static_cast<D3D11_FILL_MODE>(value);
}

} // namespace aoe