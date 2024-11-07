#pragma once

#include "../Core/ClassHelper.h"

#include "DX11GeometryPass.h"
#include "DX11LightPass.h"
#include "DX11TonePass.h"
#include "DX11DebugPass.h"
#include "CameraUtils.h"

namespace aoe {

class DX11Renderer {
AOE_NON_COPYABLE_AND_NON_MOVABLE_CLASS(DX11Renderer)

public:
	DX11Renderer() 
		: render_passes_()
		, world_(nullptr)
		, render_context_(nullptr)
	{
		render_passes_.push_back(new DX11GeometryPass());
		render_passes_.push_back(new DX11LightPass());
		render_passes_.push_back(new DX11TonePass());
		render_passes_.push_back(new DX11DebugPass());
	}

	~DX11Renderer() {
		for (DX11RenderPassBase* render_pass : render_passes_) {
			delete render_pass;
		}

		render_passes_.clear();
	}

	void Initialize(const aoe::ServiceProvider& service_provider) {
		for (DX11RenderPassBase* render_pass : render_passes_) {
			render_pass->Initialize(service_provider);
		}

		world_ = service_provider.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		render_context_ = service_provider.GetService<DX11RenderContext>();
		AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");
	}

	void Terminate() {
		for (DX11RenderPassBase* render_pass : render_passes_) {
			render_pass->Terminate();
		}
	}

	void Update() {
		if (CameraUtils::HasCamera(*world_)) {
			for (DX11RenderPassBase* render_pass : render_passes_) {
				render_pass->Update();
			}
		}
	}

	void Render() {
		PrepareRenderContext();
		UpdateCameras();

		if (CameraUtils::HasCamera(*world_)) {
			for (DX11RenderPassBase* render_pass : render_passes_) {
				render_pass->Render();
			}
		}

		PresentFrame();
		ClearContext();
	}
	
private:
	std::vector<DX11RenderPassBase*> render_passes_;

	World* world_;
	DX11RenderContext* render_context_;

	void UpdateCameras() {
		Viewport viewport = render_context_->GetViewport();
		auto filter = world_->GetFilter<CameraComponent>();

		for (Entity entity : filter) {
			auto camera_component = world_->Get<CameraComponent>(entity);
			camera_component->width = viewport.width;
			camera_component->height = viewport.height;
		}
	}

	void PrepareRenderContext() {
		render_context_->UpdateResources();

		aoe::DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
		context.SetViewport(render_context_->GetViewport());
	}

	void PresentFrame() {
		render_context_->PresentFrame();
	}

	void ClearContext() {
		const float background_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		aoe::DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
		context.ClearRenderTarget(render_context_->GetRenderTargetView(), background_color);
		context.ClearRenderTarget(render_context_->GetDiffuseTextureView(), background_color);
		context.ClearRenderTarget(render_context_->GetNormalTextureView(), background_color);
		context.ClearRenderTarget(render_context_->GetPositionTextureView(), background_color);
		context.ClearRenderTarget(render_context_->GetAccumulatorTextureView(), background_color);
		context.ClearDepth(render_context_->GetDepthBufferView(), 1.0f);
		context.ClearState();
	}
};

} // namespace aoe