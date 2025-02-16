#include "pch.h"

#include "DX11GeometryPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "RenderComponent.h"

namespace aoe {

void DX11GeometryPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		UpdateRenderData();
	}
}

void DX11GeometryPassFrameSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<TransformComponent, RenderComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(entity);
		auto render_component = GetComponent<RenderComponent>(entity);
		
		Matrix4f world = GetGlobalWorldMatrix(entity);

		TransformData transform_data{};
		transform_data.world = world.Transpose();
		transform_data.world_it = world.Inverse();

		render_component->transform_data_.Update(&transform_data);
	}
}

} // namespace aoe