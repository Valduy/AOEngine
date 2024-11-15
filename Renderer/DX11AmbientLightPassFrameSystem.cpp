#include "pch.h"

#include "DX11AmbientLightPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "AmbientLightComponent.h"

namespace aoe {

void DX11AmbientLightPassFrameSystem::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassSystemBase::Initialize(service_provider);

	InitializeAmbientLightData();
	SubscribeToComponents();
}

void DX11AmbientLightPassFrameSystem::Terminate() {
	UnsibscribeFromComponents();

	DX11RenderPassSystemBase::Terminate();
}

void DX11AmbientLightPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		UpdateRenderData();
	}
}

void DX11AmbientLightPassFrameSystem::InitializeAmbientLightData() {
	for (Entity entity : FilterEntities<AmbientLightComponent>()) {
		AddComponent<AmbientLightDataComponent>(entity);
	}
}

void DX11AmbientLightPassFrameSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<AmbientLightComponent, AmbientLightDataComponent>()) {
		auto ambient_light_component = GetComponent<AmbientLightComponent>(entity);
		auto ambient_light_data_component = GetComponent<AmbientLightDataComponent>(entity);

		AmbientLightData data{};
		data.color = ambient_light_component->color;
		data.intensity = ambient_light_component->intensity;

		ambient_light_data_component->light_data.Update(&data);
	}
}

void DX11AmbientLightPassFrameSystem::SubscribeToComponents() {
	ComponentAdded<AmbientLightComponent>().Attach(
		*this, &DX11AmbientLightPassFrameSystem::OnAmbientLightComponentAdded);
	ComponentRemoved<AmbientLightComponent>().Attach(
		*this, &DX11AmbientLightPassFrameSystem::OnAmbientLightComponentRemoved);
}

void DX11AmbientLightPassFrameSystem::UnsibscribeFromComponents() {
	ComponentAdded<AmbientLightComponent>().Detach(
		*this, &DX11AmbientLightPassFrameSystem::OnAmbientLightComponentAdded);
	ComponentRemoved<AmbientLightComponent>().Detach(
		*this, &DX11AmbientLightPassFrameSystem::OnAmbientLightComponentRemoved);
}

void DX11AmbientLightPassFrameSystem::OnAmbientLightComponentAdded(Entity entity) {
	AddComponent<AmbientLightDataComponent>(entity);
}

void DX11AmbientLightPassFrameSystem::OnAmbientLightComponentRemoved(Entity entity) {
	RemoveComponent<AmbientLightDataComponent>(entity);
}

} // namespace aoe