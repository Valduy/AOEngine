#include "pch.h"

#include "../Graphics/DX11GPUContext.h"

#include "DX11PreLightPassTickSystem.h"

namespace aoe {

DX11PreLightPassTickSystem::DX11PreLightPassTickSystem()
	: blend_state_(CreateBlendStateDescription())
{}

void DX11PreLightPassTickSystem::Update(float dt) {
	if (HasCamera()) {
		PrepareRenderContext();
	}
}

GPUBlendStateDescription DX11PreLightPassTickSystem::CreateBlendStateDescription() {
	GPUBlendStateDescription blend_state_desc{};
	blend_state_desc.is_alpha_to_coverage_enable = false;
	blend_state_desc.is_independent_blend_enable = false;
	blend_state_desc.render_targets[0].is_blend_enable = true;
	blend_state_desc.render_targets[0].source_blend = GPUBlend::kOne;
	blend_state_desc.render_targets[0].destination_blend = GPUBlend::kOne;
	blend_state_desc.render_targets[0].blend_operation = GPUBlendOperation::kAdd;
	blend_state_desc.render_targets[0].source_blend_alpha = GPUBlend::kOne;
	blend_state_desc.render_targets[0].destination_blend_alpha = GPUBlend::kOne;
	blend_state_desc.render_targets[0].blend_operation_alpha = GPUBlendOperation::kAdd;
	blend_state_desc.render_targets[0].color_write_mask = GPUColorWriteMask::kAll;

	return blend_state_desc;
}

void DX11PreLightPassTickSystem::PrepareRenderContext() {
	DX11GPURenderTargets render_targets;
	render_targets.render_target_views[0] = &GetRenderContext()->GetAccumulatorTextureView();
	render_targets.depth_stencil_view = &GetRenderContext()->GetDepthBufferView();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRenderTargets(render_targets);
	context.SetBlendState(blend_state_, 0xffffffff);
}

} // namespace aoe