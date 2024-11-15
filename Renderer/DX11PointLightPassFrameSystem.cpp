#include "pch.h"

#include "DX11PointLightPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "PointLightComponent.h"

namespace aoe {

void DX11PointLightPassFrameSystem::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassSystemBase::Initialize(service_provider);

	InitializePointLightData();
	SubscribeToComponents();
}

void DX11PointLightPassFrameSystem::Terminate() {
	UnsibscribeFromComponents();

	DX11RenderPassSystemBase::Terminate();
}

void DX11PointLightPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		UpdateRenderData(camera);
	}
}

void DX11PointLightPassFrameSystem::InitializePointLightData() {
	for (Entity entity : FilterEntities<TransformComponent, PointLightComponent>()) {
		AddComponent<PointLightDataComponent>(entity);
	}
}

void DX11PointLightPassFrameSystem::UpdateRenderData(Entity camera) {
	Vector3f camera_position = GetGlobalPosition(camera);
	Matrix4f camera_matrix = GetCameraMatrix(camera);

	for (Entity entity : FilterEntities<TransformComponent, PointLightComponent, PointLightDataComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(entity);
		auto point_light_component = GetComponent<PointLightComponent>(entity);
		auto point_light_data_component = GetComponent<PointLightDataComponent>(entity);

		Matrix4f world = GetGlobalWorldMatrix(entity);
		Matrix4f world_view_projection = camera_matrix * world;
		Matrix4f world_view_projection_t = world_view_projection.Transpose();

		PointLightData light_data{};
		light_data.view_position = camera_position;
		light_data.position = GetGlobalPosition(entity);
		light_data.color = point_light_component->color;

		point_light_data_component->transform_data.Update(&world_view_projection_t);
		point_light_data_component->light_data.Update(&light_data);
	}
}

void DX11PointLightPassFrameSystem::SubscribeToComponents() {
	ComponentAdded<TransformComponent>().Attach(
		*this, &DX11PointLightPassFrameSystem::OnTransformComponentAdded);
	ComponentAdded<PointLightComponent>().Attach(
		*this, &DX11PointLightPassFrameSystem::OnPointLightComponentAdded);

	ComponentRemoved<PointLightComponent>().Attach(
		*this, &DX11PointLightPassFrameSystem::OnComponentRemoved);
}

void DX11PointLightPassFrameSystem::UnsibscribeFromComponents() {
	ComponentAdded<TransformComponent>().Detach(
		*this, &DX11PointLightPassFrameSystem::OnTransformComponentAdded);
	ComponentAdded<PointLightComponent>().Detach(
		*this, &DX11PointLightPassFrameSystem::OnPointLightComponentAdded);

	ComponentRemoved<PointLightComponent>().Detach(
		*this, &DX11PointLightPassFrameSystem::OnComponentRemoved);
}

void DX11PointLightPassFrameSystem::OnTransformComponentAdded(Entity entity) {
	if (HasComponent<PointLightComponent>(entity)) {
		AddComponent<PointLightDataComponent>(entity);
	}
}

void DX11PointLightPassFrameSystem::OnPointLightComponentAdded(Entity entity) {
	if (HasComponent<TransformComponent>(entity)) {
		AddComponent<PointLightDataComponent>(entity);
	}
}

void DX11PointLightPassFrameSystem::OnComponentRemoved(Entity entity) {
	RemoveComponent<PointLightDataComponent>(entity);
}

} // namespace aoe