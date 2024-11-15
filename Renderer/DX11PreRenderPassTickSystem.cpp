#include "pch.h"

#include "../Graphics/DX11GPUContext.h"

#include "DX11PreRenderPassTickSystem.h"

namespace aoe {

void DX11PreRenderPassTickSystem::Update(float dt) {
	PrepareRenderContext();
	UpdateCameras();
}

void DX11PreRenderPassTickSystem::PrepareRenderContext() {
	GetRenderContext()->UpdateResources();

	DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
	context.SetViewport(GetRenderContext()->GetViewport());
}

void DX11PreRenderPassTickSystem::UpdateCameras() {
	Viewport viewport = GetRenderContext()->GetViewport();

	for (Entity entity : FilterEntities<CameraComponent>()) {
		auto camera_component = GetComponent<CameraComponent>(entity);
		camera_component->width = viewport.width;
		camera_component->height = viewport.height;
	}
}

} // namespace aoe