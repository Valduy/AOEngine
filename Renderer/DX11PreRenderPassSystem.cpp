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

	for (Entity entity : FilterEntities<TransformComponent, DX11CameraComponent>()) {
		auto camera_component = GetComponent<DX11CameraComponent>(entity);
		camera_component->width = viewport.width;
		camera_component->height = viewport.height;

		Matrix4f view_projection = GetCameraMatrix(entity);
		Matrix4f view_projection_t = view_projection.Transpose();

		Vector3fData data{};
		data.value = GetGlobalPosition(entity);
				
		camera_component->camera_data.Update(&view_projection_t);
		camera_component->transform_data.Update(&data);
	}
}

} // namespace aoe