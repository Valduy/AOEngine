#include "pch.h"

#include "DX11PointLightPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "PointLightComponent.h"

namespace aoe {

void DX11PointLightPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		UpdateRenderData();
	}
}

void DX11PointLightPassFrameSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<TransformComponent, PointLightComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(entity);
		auto point_light_component = GetComponent<PointLightComponent>(entity);

		Matrix4f world = GetGlobalWorldMatrix(entity);
		Vector3f position = GetGlobalPosition(entity);
		
		PointLightTransformData data{};
		data.world = world.Transpose();
		data.position = position;

		point_light_component->transform_data_.Update(&data);
	}
}

} // namespace aoe