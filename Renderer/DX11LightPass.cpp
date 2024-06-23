#include "pch.h"

#include "DX11LightPass.h"

namespace aoe {

DX11LightPass::DX11LightPass()
	: blend_state_(CreateBlendStateDescription())
	, ambient_light_pass_()
	, directional_light_pass_()
{}

void DX11LightPass::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassBase::Initialize(service_provider);

	ambient_light_pass_.Initialize(service_provider);
	directional_light_pass_.Initialize(service_provider);
}

void DX11LightPass::Terminate() {
	ambient_light_pass_.Terminate();
	directional_light_pass_.Terminate();
}

inline void DX11LightPass::Update() {
	ambient_light_pass_.Update();
	directional_light_pass_.Update();
}

void DX11LightPass::Render() {
	PrepareRenderContext();

	ambient_light_pass_.Render();
	directional_light_pass_.Render();
}

GPUBlendStateDescription DX11LightPass::CreateBlendStateDescription() {
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

void DX11LightPass::PrepareRenderContext() {
	DX11GPURenderTargets render_targets;
	render_targets.render_target_views[0] = &GetRenderContext()->GetAccumulatorTextureView();
	render_targets.depth_stencil_view = &GetRenderContext()->GetDepthBufferView();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRenderTargets(render_targets);
	context.SetBlendState(blend_state_, 0xffffffff);
}

} // namespace aoe