#include "pch.h"

#include "DX11GPUBlendState.h"
#include "DX11GPUDevice.h"
#include "DX11Helper.h"

namespace aoe {

aoe::DX11GPUBlendState::DX11GPUBlendState(const GPUBlendStateDescription description)
	: description_(description)
{
	D3D11_BLEND_DESC blend_state_desc{};
	blend_state_desc.AlphaToCoverageEnable = description_.is_alpha_to_coverage_enable;
	blend_state_desc.IndependentBlendEnable = description_.is_independent_blend_enable;

	for (size_t i = 0; i < DX11GPURenderTargets::kRenderTargetsCount; ++i) {
		blend_state_desc.RenderTarget[i].BlendEnable = description_.render_targets[i].is_blend_enable;
		blend_state_desc.RenderTarget[i].SrcBlend = ToDXBlend(description_.render_targets[i].source_blend);
		blend_state_desc.RenderTarget[i].DestBlend = ToDXBlend(description_.render_targets[i].destination_blend);
		blend_state_desc.RenderTarget[i].BlendOp = ToDXBlendOperation(description_.render_targets[i].blend_operation);
		blend_state_desc.RenderTarget[i].SrcBlendAlpha = ToDXBlend(description_.render_targets[i].source_blend_alpha);
		blend_state_desc.RenderTarget[i].DestBlendAlpha = ToDXBlend(description_.render_targets[i].destination_blend_alpha);
		blend_state_desc.RenderTarget[i].BlendOpAlpha = ToDXBlendOperation(description_.render_targets[i].blend_operation_alpha);
		blend_state_desc.RenderTarget[i].RenderTargetWriteMask = description_.render_targets[i].render_target_write_mask;
	}

	const HRESULT hr = DX11GPUDevice::Instance().GetNative()->CreateBlendState(&blend_state_desc, blend_state_.GetAddressOf());
	AOE_DX_TRY_LOG_ERROR_AND_THROW(hr, "Failed to create blend state.");
}

ID3D11BlendState* aoe::DX11GPUBlendState::GetNative() const {
	return blend_state_.Get();
}

const GPUBlendStateDescription& aoe::DX11GPUBlendState::GetDescription() const {
	return description_;
}

D3D11_BLEND DX11GPUBlendState::ToDXBlend(const GPUBlend value) {
	return static_cast<D3D11_BLEND>(value);
}

D3D11_BLEND_OP DX11GPUBlendState::ToDXBlendOperation(const GPUBlendOperation value) {
	return static_cast<D3D11_BLEND_OP>(value);
}

} // namespace aoe
