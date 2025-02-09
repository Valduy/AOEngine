#include "pch.h"

#include "DX11GeometryPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "RenderComponent.h"

namespace aoe {

void DX11GeometryPassFrameSystem::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassSystemBase::Initialize(service_provider);

	InitializeGeometryData();
	SubscribeToComponents();
}

void DX11GeometryPassFrameSystem::Terminate() {
	UnsibscribeFromComponents();

	DX11RenderPassSystemBase::Terminate();
}

void DX11GeometryPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		UpdateRenderData(camera);
	}
}

void DX11GeometryPassFrameSystem::InitializeGeometryData() {
	for (Entity entity : FilterEntities<TransformComponent, RenderComponent>()) {
		AddComponent<GeometryDataComponent>(entity);
	}
}

void DX11GeometryPassFrameSystem::UpdateRenderData(Entity camera) {
	Matrix4f camera_matrix = GetCameraMatrix(camera);

	for (Entity entity : FilterEntities<TransformComponent, RenderComponent, GeometryDataComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(entity);
		auto render_component = GetComponent<RenderComponent>(entity);
		auto geometry_data_component = GetComponent<GeometryDataComponent>(entity);

		Matrix4f world = GetGlobalWorldMatrix(entity);
		Matrix4f world_view_projection = camera_matrix * world;

		TransformData transform_data{};
		transform_data.world = world.Transpose();
		transform_data.world_view_projection = world_view_projection.Transpose();
		transform_data.inverse_transpose_world = world.Inverse();

		geometry_data_component->transform_data.Update(&transform_data);
	}
}

void DX11GeometryPassFrameSystem::SubscribeToComponents() {
	ComponentAdded<TransformComponent>().Attach(
		*this, &DX11GeometryPassFrameSystem::OnTransformComponentAdded);
	ComponentAdded<RenderComponent>().Attach(
		*this, &DX11GeometryPassFrameSystem::OnRenderComponentAdded);

	ComponentRemoved<RenderComponent>().Attach(
		*this, &DX11GeometryPassFrameSystem::OnComponentRemoved);
}

void DX11GeometryPassFrameSystem::UnsibscribeFromComponents() {
	ComponentAdded<TransformComponent>().Detach(
		*this, &DX11GeometryPassFrameSystem::OnTransformComponentAdded);
	ComponentAdded<RenderComponent>().Detach(
		*this, &DX11GeometryPassFrameSystem::OnRenderComponentAdded);

	ComponentRemoved<RenderComponent>().Detach(
		*this, &DX11GeometryPassFrameSystem::OnComponentRemoved);
}

void DX11GeometryPassFrameSystem::OnTransformComponentAdded(Entity entity) {
	if (HasComponent<RenderComponent>(entity)) {
		AddComponent<GeometryDataComponent>(entity);
	}
}

void DX11GeometryPassFrameSystem::OnRenderComponentAdded(Entity entity) {
	if (HasComponent<TransformComponent>(entity)) {
		AddComponent<GeometryDataComponent>(entity);
	}
}

void DX11GeometryPassFrameSystem::OnComponentRemoved(Entity entity) {
	RemoveComponent<GeometryDataComponent>(entity);
}

} // namespace aoe