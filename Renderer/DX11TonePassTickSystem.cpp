#include "pch.h"

#include "DX11TonePassTickSystem.h"

namespace aoe {

DX11TonePassTickSystem::DX11TonePassTickSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/TonePass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/TonePass.hlsl"))
	, sampler_(CreateSamplerDescription())
	, blend_state_(CreateBlendStateDescription())
{}

void DX11TonePassTickSystem::Update(float dt) {
	if (HasCamera()) {
		Render();
	}
}

GPUSamplerDescription DX11TonePassTickSystem::CreateSamplerDescription() {
	return {};
}

GPUBlendStateDescription DX11TonePassTickSystem::CreateBlendStateDescription() {
	GPUBlendStateDescription blend_state_desc{};
	blend_state_desc.is_alpha_to_coverage_enable = false;
	blend_state_desc.is_independent_blend_enable = false;
	blend_state_desc.render_targets[0].is_blend_enable = true;
	blend_state_desc.render_targets[0].source_blend = GPUBlend::kOne;
	blend_state_desc.render_targets[0].destination_blend = GPUBlend::kZero;
	blend_state_desc.render_targets[0].blend_operation = GPUBlendOperation::kAdd;
	blend_state_desc.render_targets[0].source_blend_alpha = GPUBlend::kOne;
	blend_state_desc.render_targets[0].destination_blend_alpha = GPUBlend::kZero;
	blend_state_desc.render_targets[0].blend_operation_alpha = GPUBlendOperation::kAdd;
	blend_state_desc.render_targets[0].color_write_mask = GPUColorWriteMask::kAll;

	return blend_state_desc;
}

void DX11TonePassTickSystem::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.Draw(4);
}

void DX11TonePassTickSystem::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = GetRenderContext()->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ false, GPUDepthWriteMask::kWriteNone, GPUComparsionFunction::kNever };
	const DX11GPUDepthState& depth_state = GetRenderContext()->GetDepthState(ds_id);

	DX11GPURenderTargets render_targets;
	render_targets.render_target_views[0] = &GetRenderContext()->GetRenderTargetView();
	render_targets.depth_stencil_view = &GetRenderContext()->GetDepthBufferView();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRasterizerState(rasterizer_state);
	context.SetBlendState(blend_state_, 0xffffffff);
	context.SetDepthState(depth_state);
	context.SetVertexShader(vertex_shader_);
	context.SetPixelShader(pixel_shader_);
	context.SetRenderTargets(render_targets);
	context.SetShaderResource(GPUShaderType::kPixel, GetRenderContext()->GetAccumulatorTextureView(), 0);
	context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleStrip);
}

} // namespace aoe