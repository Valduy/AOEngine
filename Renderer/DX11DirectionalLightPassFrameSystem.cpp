#include "pch.h"

#include "DX11DirectionalLightPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "DirectionalLightComponent.h"

namespace aoe {

void DX11DirectionalLightPassFrameSystem::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassSystemBase::Initialize(service_provider);

	InitializeDirectionLightData();
	SubscribeToComponents();
}

void DX11DirectionalLightPassFrameSystem::Terminate() {
	UnsibscribeFromComponents();

	DX11RenderPassSystemBase::Terminate();
}

void DX11DirectionalLightPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		UpdateRenderData(camera);
	}
}

void DX11DirectionalLightPassFrameSystem::InitializeDirectionLightData() {
	for (Entity entity : FilterEntities<DirectionalLightComponent>()) {
		AddComponent<DirectionalLightDataComponent>(entity);
	}
}

void DX11DirectionalLightPassFrameSystem::UpdateRenderData(Entity camera) {
	Vector3f camera_position = GetGlobalPosition(camera);

	for (Entity entity : FilterEntities<TransformComponent, DirectionalLightComponent, DirectionalLightDataComponent>()) {
		auto directional_light_component = GetComponent<DirectionalLightComponent>(entity);
		auto directional_light_data_component = GetComponent<DirectionalLightDataComponent>(entity);

		DirectionalLightData data{};
		data.view_position = camera_position;
		data.direction = GetGlobalTransform(entity).GetForward();
		data.color = directional_light_component->color;

		directional_light_data_component->light_data.Update(&data);
	}
}

void DX11DirectionalLightPassFrameSystem::SubscribeToComponents() {
	ComponentAdded<DirectionalLightComponent>().Attach(
		*this, &DX11DirectionalLightPassFrameSystem::OnDirectionLightComponentAdded);
	ComponentRemoved<DirectionalLightComponent>().Attach(
		*this, &DX11DirectionalLightPassFrameSystem::OnDirectionLightComponentRemoved);
}

void DX11DirectionalLightPassFrameSystem::UnsibscribeFromComponents() {
	ComponentAdded<DirectionalLightComponent>().Detach(
		*this, &DX11DirectionalLightPassFrameSystem::OnDirectionLightComponentAdded);
	ComponentRemoved<DirectionalLightComponent>().Detach(
		*this, &DX11DirectionalLightPassFrameSystem::OnDirectionLightComponentRemoved);
}

void DX11DirectionalLightPassFrameSystem::OnDirectionLightComponentAdded(Entity entity) {
	AddComponent<DirectionalLightDataComponent>(entity);
}

void DX11DirectionalLightPassFrameSystem::OnDirectionLightComponentRemoved(Entity entity) {
	RemoveComponent<DirectionalLightDataComponent>(entity);
}

} // namespace aoe