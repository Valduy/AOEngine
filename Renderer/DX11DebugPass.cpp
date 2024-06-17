#include "pch.h"

#include "DX11DebugPass.h"
#include "DX11RenderDataComponents.h"
#include "LineComponent.h"
#include "CameraUtils.h"

namespace aoe {

DX11DebugPass::DX11DebugPass()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DebugPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DebugPass.hlsl"))
{}

void DX11DebugPass::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassBase::Initialize(service_provider);

	InitializeLineData();
	SubscribeToComponents();
}

void DX11DebugPass::Terminate() {
	UnsibscribeFromComponents();
}

void DX11DebugPass::Update() {
	Entity camera = CameraUtils::GetActualCamera(*GetWorld());
	UpdateLineData(camera);
}

void DX11DebugPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	auto filter = GetWorld()->GetFilter<LineComponent, DX11LineDataComponent, DX11LineResourcesComponent>();

	for (Entity entity : filter) {
		auto line_data_component = GetWorld()->Get<DX11LineDataComponent>(entity);
		auto line_resources_component = GetWorld()->Get<DX11LineResourcesComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kVertex, line_data_component->buffer);

		for (const DX11GPUBuffer& vertex_buffer : line_resources_component->GetLineResources()) {
			context.SetVertexBuffer(vertex_buffer);
			context.Draw(vertex_buffer.GetElementsCount());
		}
	}
}

void DX11DebugPass::InitializeLineData() {
	auto filter = GetWorld()->GetFilter<TransformComponent, LineComponent>();

	for (Entity entity : filter) {
		auto line_component = GetWorld()->Get<LineComponent>(entity);

		GetWorld()->Add<DX11LineDataComponent>(entity);
		GetWorld()->Add<DX11LineResourcesComponent>(entity, CreateLineResources(line_component->GetLine()));
	}
}

std::vector<DX11GPUBuffer> DX11DebugPass::CreateLineResources(const Line& lines) {
	std::vector<DX11GPUBuffer> line_resources;

	for (const Segment& segment : lines.GetSegments()) {
		line_resources.emplace_back(CreateVertexBuffer(segment));
	}

	return line_resources;
}

DX11GPUBuffer DX11DebugPass::CreateVertexBuffer(const Segment& segment) {
	const aoe::GPUBufferDescription vertex_buffer_desc{
		aoe::GPUBufferType::kVertexBuffer,
		aoe::GPUResourceUsage::kDefault
	};

	const std::vector<Vector3f>& points = segment.GetPoints();
	return DX11GPUBuffer::Create<Vector3f>(vertex_buffer_desc, points.data(), points.size());
}

void DX11DebugPass::SubscribeToComponents() {
	GetWorld()->ComponentAdded<TransformComponent>().Attach(*this, &DX11DebugPass::OnTransformComponentAdded);
	GetWorld()->ComponentRemoved<TransformComponent>().Attach(*this, &DX11DebugPass::OnComponentRemoved);

	GetWorld()->ComponentAdded<LineComponent>().Attach(*this, &DX11DebugPass::OnLineComponentAdded);
	GetWorld()->ComponentRemoved<LineComponent>().Attach(*this, &DX11DebugPass::OnComponentRemoved);
}

void DX11DebugPass::UnsibscribeFromComponents() {
	GetWorld()->ComponentAdded<TransformComponent>().Detach(*this, &DX11DebugPass::OnTransformComponentAdded);
	GetWorld()->ComponentRemoved<TransformComponent>().Detach(*this, &DX11DebugPass::OnComponentRemoved);

	GetWorld()->ComponentAdded<LineComponent>().Detach(*this, &DX11DebugPass::OnLineComponentAdded);
	GetWorld()->ComponentRemoved<LineComponent>().Detach(*this, &DX11DebugPass::OnComponentRemoved);
}

void DX11DebugPass::UpdateLineData(Entity camera) {
	Matrix4f camera_matrix = CameraUtils::GetCameraMatrix(*GetWorld(), camera);
	auto filter = GetWorld()->GetFilter<TransformComponent, LineComponent, DX11LineDataComponent>();

	for (Entity entity : filter) {
		auto line_component = GetWorld()->Get<LineComponent>(entity);
		auto line_data_component = GetWorld()->Get<DX11LineDataComponent>(entity);

		Matrix4f world = TransformUtils::GetGlobalWorldMatrix(*GetWorld(), *GetRelationeer(), entity);
		Matrix4f world_view_projection = camera_matrix * world;

		LineData line_data{};
		line_data.world_view_projection = world_view_projection.Transpose();
		line_data.color = line_component->color;

		line_data_component->Update(&line_data);
	}
}

void DX11DebugPass::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = GetRenderContext()->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteAll, GPUComparsionFunction::kLess };
	const DX11GPUDepthState& depth_state = GetRenderContext()->GetDepthState(ds_id);

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRasterizerState(rasterizer_state);
	context.SetDepthState(depth_state);
	context.SetVertexShader(vertex_shader_);
	context.SetPixelShader(pixel_shader_);
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kLineStrip);
}

void DX11DebugPass::OnTransformComponentAdded(Entity entity) {
	if (GetWorld()->Has<LineComponent>(entity)) {
		SetupLineEntity(entity);
	}
}

void DX11DebugPass::OnLineComponentAdded(Entity entity) {
	if (GetWorld()->Has<TransformComponent>(entity)) {
		SetupLineEntity(entity);
	}
}

void DX11DebugPass::OnComponentRemoved(Entity entity) {
	GetWorld()->Remove<DX11LineDataComponent>(entity);
	GetWorld()->Remove<DX11LineResourcesComponent>(entity);
}

void DX11DebugPass::SetupLineEntity(Entity entity) {
	AOE_ASSERT_MSG(!GetWorld()->Has<DX11LineDataComponent>(entity), "Entity already has DX11LineDataComponent.");
	AOE_ASSERT_MSG(!GetWorld()->Has<DX11LineResourcesComponent>(entity), "Entity already has DX11LineResourcesComponent.");

	auto line_component = GetWorld()->Get<LineComponent>(entity);

	GetWorld()->Add<DX11LineDataComponent>(entity);
	GetWorld()->Add<DX11LineResourcesComponent>(entity, CreateLineResources(line_component->GetLine()));
}

} // namespace aoe