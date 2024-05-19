#pragma once

#include "../Core/Debug.h"
#include "../Game/ServiceProvider.h"
#include "../Graphics/DX11GPUContext.h"

#include "IRenderPass.h"
#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"

namespace aoe {

class DX11TonePass : public IRenderPass {
public:
	DX11TonePass(const ServiceProvider& service_provider)
		: service_provider_(service_provider)
		, render_targets_()
		, vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/TonePass.hlsl"))
		, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/TonePass.hlsl"))
		, sampler_(CreateSamplerDescription())
		, blend_state_(CreateBlendStateDescription())
		, render_context_(nullptr)
	{}

	void Initialize() override {
		render_context_ = service_provider_.GetService<DX11RenderContext>();
		AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

		InitializeRenderTargets();
	}

	void Render() override {
		PrepareRenderContext();

		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.Draw(4);
	}

private:
	const ServiceProvider& service_provider_;

	DX11GPURenderTargets render_targets_;
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;

	DX11RenderContext* render_context_;

	static GPUSamplerDescription CreateSamplerDescription() {
		return {};
	}

	static GPUBlendStateDescription CreateBlendStateDescription() {
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

	void InitializeRenderTargets() {
		render_targets_.render_target_views[0] = &render_context_->GetRenderTargetView();
		render_targets_.depth_stencil_view = &render_context_->GetDepthBufferView();
	}

	void PrepareRenderContext() {
		DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
		const DX11GPURasterizerState& rasterizer_state = render_context_->GetRasterizerState(rs_id);

		DX11DepthStateID ds_id{ false, GPUDepthWriteMask::kWriteNone, GPUComparsionFunction::kNever };
		const DX11GPUDepthState& depth_state = render_context_->GetDepthState(ds_id);

		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRasterizerState(rasterizer_state);
		context.SetBlendState(blend_state_, 0xffffffff);
		context.SetDepthState(depth_state);
		context.SetVertexShader(vertex_shader_);
		context.SetPixelShader(pixel_shader_);
		context.SetRenderTargets(render_targets_);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_->GetAccumulatorTextureView(), 0);
		context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
		context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleStrip);
	}
};

} // namespace aoe