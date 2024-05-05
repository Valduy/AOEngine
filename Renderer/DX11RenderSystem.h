#pragma once

#include "../Game/IECSSystem.h"

#include "DX11GeometryPass.h"
#include "DX11LightPass.h"
#include "DX11TonePass.h"
#include "CameraUtils.h"

namespace aoe {

class DX11RenderSystem : public IECSSystem {
public:
	DX11RenderSystem(const Window& window, const aoe::ServiceProvider& service_provider)
		: service_provider_(service_provider)
		, geometry_pass_(service_provider)
		, light_pass_(service_provider)
		, tone_pass_(service_provider)
		, world_(nullptr)
		, render_context_(nullptr)
	{}

	void Initialize() override {
		geometry_pass_.Initialize();
		light_pass_.Initialize();
		tone_pass_.Initialize();

		world_ = service_provider_.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		render_context_ = service_provider_.GetService<DX11RenderContext>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no DX11RenderContext service.");

	}

	void PerTickUpdate(float dt) override {
		PrepareContext();

		// TODO: may be use it for viewport setup
		// TODO: check camera deletation in run time
		Entity camera = CameraUtils::GetActualCamera(*world_);

		if (camera.IsNull()) {
			return;
		}

		geometry_pass_.Render();
		light_pass_.Render();
		tone_pass_.Render();
		PresentFrame();
	}

	void PerFrameUpdate(float dt) override {}

private:
	const ServiceProvider& service_provider_;
	
	DX11GeometryPass geometry_pass_;
	DX11LightPass light_pass_;
	DX11TonePass tone_pass_;

	World* world_;
	DX11RenderContext* render_context_;

	void PrepareContext() {
		const float background_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		aoe::DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
		context.ClearRenderTarget(render_context_->GetRenderTargetView(), background_color);
		context.ClearRenderTarget(render_context_->GetDiffuseTextureView(), background_color);
		context.ClearRenderTarget(render_context_->GetNormalTextureView(), background_color);
		context.ClearRenderTarget(render_context_->GetPositionTextureView(), background_color);
		context.ClearRenderTarget(render_context_->GetAccumulatorTextureView(), background_color);
		context.ClearDepth(render_context_->GetDepthBufferView(), 1.0f);
		context.ClearState();
		context.SetViewport(render_context_->GetViewport());
	}

	void PresentFrame() {
		render_context_->PresentFrame();
	}
};

} // namespace aoe