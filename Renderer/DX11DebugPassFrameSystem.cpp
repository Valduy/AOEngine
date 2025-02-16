#include "pch.h"

#include "DX11DebugPassFrameSystem.h"
#include "LineComponent.h"

namespace aoe {

void DX11DebugPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		UpdateRenderData();
	}
}

void DX11DebugPassFrameSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<TransformComponent, LineComponent>()) {
		auto line_component = GetComponent<LineComponent>(entity);
		
		Matrix4f world = GetGlobalWorldMatrix(entity);
		Matrix4f world_t = world.Transpose();
		
		line_component->transform_data_.Update(&world_t);
	}
}

} // namespace aoe