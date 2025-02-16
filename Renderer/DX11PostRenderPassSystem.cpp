#include "pch.h"

#include "../Graphics/DX11GPUContext.h"

#include "DX11PostRenderPassSystem.h"

namespace aoe {

void DX11PostRenderPassSystem::Update(float dt) {
	PresentFrame();
	ClearContext();
}

void DX11PostRenderPassSystem::PresentFrame() {
	GetRenderContext()->PresentFrame();
}

void DX11PostRenderPassSystem::ClearContext() {
	const float background_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

	aoe::DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
	context.ClearRenderTarget(GetRenderContext()->GetRenderTargetView(), background_color);
	context.ClearRenderTarget(GetRenderContext()->GetDiffuseTextureView(), background_color);
	context.ClearRenderTarget(GetRenderContext()->GetSpecularTextureView(), background_color);
	context.ClearRenderTarget(GetRenderContext()->GetNormalTextureView(), background_color);
	context.ClearRenderTarget(GetRenderContext()->GetPositionTextureView(), background_color);
	context.ClearRenderTarget(GetRenderContext()->GetAccumulatorTextureView(), background_color);
	context.ClearDepth(GetRenderContext()->GetDepthBufferView(), 1.0f);
	context.ClearState();
}

} // namespace aoe