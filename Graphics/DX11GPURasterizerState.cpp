#include "pch.h"

#include "../Core/Debug.h"

#include "DX11GPURasterizerState.h"
#include "DX11Helper.h"

namespace aoe {

DX11GPURasterizerState::DX11GPURasterizerState(const GPURasterizerStateDescription& description)
	: description_(description)
	, rasterizer_state_(nullptr)
{
	D3D11_RASTERIZER_DESC rasterizer_desc{};
	rasterizer_desc.CullMode = ToDXCullMode(description_.cull_mode);
	rasterizer_desc.FillMode = ToDXFillMode(description_.fill_mode);

	const HRESULT hr = DX11GPUDevice::Instance().GetNative()->CreateRasterizerState(&rasterizer_desc, rasterizer_state_.GetAddressOf());
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create rasterizer state.");
}

ID3D11RasterizerState* DX11GPURasterizerState::GetNative() const {
	return rasterizer_state_.Get();
}

const GPURasterizerStateDescription& DX11GPURasterizerState::GetDescription() const {
	return description_;
}

D3D11_CULL_MODE DX11GPURasterizerState::ToDXCullMode(const GPUCullMode value) {
	return static_cast<D3D11_CULL_MODE>(value);
}

D3D11_FILL_MODE DX11GPURasterizerState::ToDXFillMode(const GPUFillMode value) {
	return static_cast<D3D11_FILL_MODE>(value);
}

} // namespace aoe