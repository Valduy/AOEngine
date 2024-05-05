#pragma once

#include "DX11AmbientLightPass.h"
#include "DX11DirectionalLightPass.h"

namespace aoe {

class DX11LightPass : public IRenderPass {
public:
	DX11LightPass(const ServiceProvider& service_provider)
		: service_provider_(service_provider)
		, render_targets_()
		, blend_state_(CreateBlendStateDescription())
		, ambient_light_pass_(service_provider)
		, directional_light_pass_(service_provider)
		, world_(nullptr)
		, render_context_(nullptr)
	{}

	void Initialize() override {
		world_ = service_provider_.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		render_context_ = service_provider_.GetService<DX11RenderContext>();
		AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

		InitializeRenderTargets();

		ambient_light_pass_.Initialize();
		directional_light_pass_.Initialize();
	}

	void Render() override {
		PrepareRenderContext();

		ambient_light_pass_.Render();
		directional_light_pass_.Render();
	}

private:
	const ServiceProvider& service_provider_;

	DX11GPURenderTargets render_targets_;
	DX11GPUBlendState blend_state_;
	DX11AmbientLightPass ambient_light_pass_;
	DX11DirectionalLightPass directional_light_pass_;

	World* world_;
	DX11RenderContext* render_context_;

	static GPUBlendStateDescription CreateBlendStateDescription() {
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

	void InitializeRenderTargets() {
		render_targets_.render_target_views[0] = &render_context_->GetAccumulatorTextureView();
		render_targets_.depth_stencil_view = &render_context_->GetDepthBufferView();
	}

	void PrepareRenderContext() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRenderTargets(render_targets_);
		context.SetBlendState(blend_state_, 0xffffffff);
	}
};

} // namespace aoe