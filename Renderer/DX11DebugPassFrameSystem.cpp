#include "pch.h"

#include "DX11DebugPassFrameSystem.h"
#include "DX11RenderDataComponents.h"
#include "DX11LineResourcesComponent.h"
#include "LineComponent.h"

namespace aoe {

void DX11DebugPassFrameSystem::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassSystemBase::Initialize(service_provider);

	InitializeLineData();
	SubscribeToComponents();
}

void DX11DebugPassFrameSystem::Terminate() {
	UnsibscribeFromComponents();

	DX11RenderPassSystemBase::Terminate();
}

void DX11DebugPassFrameSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		UpdateRenderData(camera);
	}
}

void DX11DebugPassFrameSystem::InitializeLineData() {
	for (Entity entity : FilterEntities<TransformComponent, LineComponent>()) {
		auto line_component = GetComponent<LineComponent>(entity);

		AddComponent<LineDataComponent>(entity);
		AddComponent<DX11LineResourcesComponent>(entity, CreateLineResources(line_component->GetLine()));
	}
}

void DX11DebugPassFrameSystem::UpdateRenderData(Entity camera) {
	Matrix4f camera_matrix = GetCameraMatrix(camera);

	for (Entity entity : FilterEntities<TransformComponent, LineComponent, LineDataComponent>()) {
		auto line_component = GetComponent<LineComponent>(entity);
		auto line_data_component = GetComponent<LineDataComponent>(entity);

		Matrix4f world = GetGlobalWorldMatrix(entity);
		Matrix4f world_view_projection = camera_matrix * world;

		LineData line_data{};
		line_data.world_view_projection = world_view_projection.Transpose();
		line_data.color = line_component->color;

		line_data_component->line_data.Update(&line_data);
	}
}

std::vector<DX11GPUBuffer> DX11DebugPassFrameSystem::CreateLineResources(const Line& lines) {
	std::vector<DX11GPUBuffer> line_resources;

	for (const Segment& segment : lines.GetSegments()) {
		line_resources.emplace_back(CreateVertexBuffer(segment));
	}

	return line_resources;
}

DX11GPUBuffer DX11DebugPassFrameSystem::CreateVertexBuffer(const Segment& segment) {
	const aoe::GPUBufferDescription vertex_buffer_desc{
		aoe::GPUBufferType::kVertexBuffer,
		aoe::GPUResourceUsage::kDefault
	};

	const std::vector<Vector3f>& points = segment.GetPoints();
	return DX11GPUBuffer::Create<Vector3f>(vertex_buffer_desc, points.data(), points.size());
}

void DX11DebugPassFrameSystem::SubscribeToComponents() {
	ComponentAdded<TransformComponent>().Attach(
		*this, &DX11DebugPassFrameSystem::OnTransformComponentAdded);
	ComponentRemoved<TransformComponent>().Attach(
		*this, &DX11DebugPassFrameSystem::OnComponentRemoved);

	ComponentAdded<LineComponent>().Attach(
		*this, &DX11DebugPassFrameSystem::OnLineComponentAdded);
	ComponentRemoved<LineComponent>().Attach(
		*this, &DX11DebugPassFrameSystem::OnComponentRemoved);
}

void DX11DebugPassFrameSystem::UnsibscribeFromComponents() {
	ComponentAdded<TransformComponent>().Detach(
		*this, &DX11DebugPassFrameSystem::OnTransformComponentAdded);
	ComponentRemoved<TransformComponent>().Detach(
		*this, &DX11DebugPassFrameSystem::OnComponentRemoved);

	ComponentAdded<LineComponent>().Detach(
		*this, &DX11DebugPassFrameSystem::OnLineComponentAdded);
	ComponentRemoved<LineComponent>().Detach(
		*this, &DX11DebugPassFrameSystem::OnComponentRemoved);
}

void DX11DebugPassFrameSystem::OnTransformComponentAdded(Entity entity) {
	if (HasComponent<LineComponent>(entity)) {
		SetupLineEntity(entity);
	}
}

void DX11DebugPassFrameSystem::OnLineComponentAdded(Entity entity) {
	if (HasComponent<TransformComponent>(entity)) {
		SetupLineEntity(entity);
	}
}

void DX11DebugPassFrameSystem::OnComponentRemoved(Entity entity) {
	RemoveComponent<LineDataComponent>(entity);
	RemoveComponent<DX11LineResourcesComponent>(entity);
}

void DX11DebugPassFrameSystem::SetupLineEntity(Entity entity) {
	AOE_ASSERT_MSG(!HasComponent<LineDataComponent>(entity), "Entity already has LineDataComponent.");
	AOE_ASSERT_MSG(!HasComponent<DX11LineResourcesComponent>(entity), "Entity already has DX11LineResourcesComponent.");

	auto line_component = GetComponent<LineComponent>(entity);

	AddComponent<LineDataComponent>(entity);
	AddComponent<DX11LineResourcesComponent>(entity, CreateLineResources(line_component->GetLine()));
}

} // namespace aoe