#include "pch.h"

#include "../Graphics/DX11GPUContext.h"

#include "DX11PreRenderPassSystem.h"

namespace aoe {

void DX11PreRenderPassSystem::Update(float dt) {
	PrepareRenderContext();
	UpdateCameras();
}

void DX11PreRenderPassSystem::PrepareRenderContext() {
	GetRenderContext()->UpdateResources();

	DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
	context.SetViewport(GetRenderContext()->GetViewport());
}

void DX11PreRenderPassSystem::UpdateCameras() {
	Viewport viewport = GetRenderContext()->GetViewport();

	for (Entity entity : FilterEntities<CameraComponent>()) {
		auto camera_component = GetComponent<CameraComponent>(entity);
		camera_component->width = viewport.width;
		camera_component->height = viewport.height;
	}
}

} // namespace aoe