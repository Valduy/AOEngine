#include "pch.h"

#include "DX11DirectionalLightPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "DirectionalLightComponent.h"

namespace aoe {

void DX11DirectionalLightPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		UpdateRenderData();
	}
}

void DX11DirectionalLightPassFrameSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<TransformComponent, DirectionalLightComponent>()) {
		auto directional_light_component = GetComponent<DirectionalLightComponent>(entity);

		Vector3fData data{};
		data.value = GetGlobalTransform(entity).GetForward();

		directional_light_component->transform_data_.Update(&data);
	}
}

} // namespace aoe