#include "pch.h"

#include "DX11RenderSystem.h"
#include "CameraUtils.h"

namespace aoe {

DX11RenderSystem::DX11RenderSystem()
	: geometry_pass_()
	, light_pass_()
	, tone_pass_()
	, debug_pass_()
	, render_context_(nullptr)
{}

void DX11RenderSystem::Initialize(const aoe::ServiceProvider& service_provider) {
	ECSSystemBase::Initialize(service_provider);

	geometry_pass_.Initialize(service_provider);
	light_pass_.Initialize(service_provider);
	tone_pass_.Initialize(service_provider);
	debug_pass_.Initialize(service_provider);

	render_context_ = service_provider.GetService<DX11RenderContext>();
	AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");
}

void DX11RenderSystem::Terminate() {
	geometry_pass_.Terminate();
	light_pass_.Terminate();
	tone_pass_.Terminate();
	debug_pass_.Terminate();
}

void DX11RenderSystem::PerTickUpdate(float dt) {
	Entity camera = CameraUtils::GetActualCamera(*GetWorld());

	PrepareRenderContext();

	if (!camera.IsNull()) {	
		Render();
	}

	PresentFrame();
}

void DX11RenderSystem::PerFrameUpdate(float dt) {
	Entity camera = CameraUtils::GetActualCamera(*GetWorld());

	if (camera.IsNull()) {
		return;
	}

	geometry_pass_.Update();
	light_pass_.Update();
	debug_pass_.Update();
}

void DX11RenderSystem::PrepareRenderContext() {
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

void DX11RenderSystem::Render() {
	geometry_pass_.Render();
	light_pass_.Render();
	tone_pass_.Render();
	debug_pass_.Render();
}

void DX11RenderSystem::PresentFrame() {
	render_context_->PresentFrame();
}

} // namespace aoe