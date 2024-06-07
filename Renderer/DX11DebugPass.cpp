#include "pch.h"

#include "DX11DebugPass.h"
#include "DX11RenderDataComponents.h"

namespace aoe {

DX11DebugPass::DX11DebugPass(const ServiceProvider& service_provider)
	: service_provider_(service_provider)
	, vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DebugPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DebugPass.hlsl"))
	, vertex_buffers_()
	, world_(nullptr)
	, render_context_(nullptr)
	, relationeer_(nullptr)
{}

void DX11DebugPass::Initialize() {
	world_ = service_provider_.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

	render_context_ = service_provider_.GetService<DX11RenderContext>();
	AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

	relationeer_ = service_provider_.GetService<Relationeer<TransformComponent>>();
	AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");

	InitializeLineData();

	SubscribeToComponents();
}

void DX11DebugPass::Terminate() {
	UnsibscribeFromComponents();
}

void DX11DebugPass::Update() {
	Entity camera = CameraUtils::GetActualCamera(*world_);
	UpdateLineData(camera);
}

void DX11DebugPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	world_->ForEach<LineComponent, DX11LineDataComponent>(
	[&, this](auto entity, auto line_component, auto line_data_component) {
		auto it = vertex_buffers_.find(entity);

		if (it == vertex_buffers_.end()) {
			AOE_LOG_WARNING("There is no vertex buffer for entity.");
			return;
		}
		
		context.SetVertexBuffer(it->second);
		context.SetConstantBuffer(GPUShaderType::kVertex, line_data_component->buffer);
		context.Draw(line_component->points.size());
	});
}

void DX11DebugPass::InitializeLineData() {
	world_->ForEach<TransformComponent, LineComponent>(
	[this](auto entity, auto transform_component, auto line_component) {
		world_->Add<DX11LineDataComponent>(entity);
		vertex_buffers_.emplace(entity, CreateVertexBuffer(line_component->points));
	});
}

DX11GPUBuffer DX11DebugPass::CreateVertexBuffer(const std::vector<Vector3f>& points) {
	const aoe::GPUBufferDescription vertex_buffer_desc{
		aoe::GPUBufferType::kVertexBuffer,
		aoe::GPUResourceUsage::kDefault
	};

	return DX11GPUBuffer::Create<Vector3f>(vertex_buffer_desc, points.data(), points.size());
}

void DX11DebugPass::SubscribeToComponents() {
	world_->ComponentAdded<TransformComponent>().Attach(*this, &DX11DebugPass::OnTransformComponentAdded);
	world_->ComponentRemoved<TransformComponent>().Attach(*this, &DX11DebugPass::OnTransformComponentRemoved);

	world_->ComponentAdded<LineComponent>().Attach(*this, &DX11DebugPass::OnLineComponentAdded);
	world_->ComponentRemoved<LineComponent>().Attach(*this, &DX11DebugPass::OnLineComponentRemoved);
}

void DX11DebugPass::UnsibscribeFromComponents() {
	world_->ComponentAdded<TransformComponent>().Detach(*this, &DX11DebugPass::OnTransformComponentAdded);
	world_->ComponentRemoved<TransformComponent>().Detach(*this, &DX11DebugPass::OnTransformComponentRemoved);

	world_->ComponentAdded<LineComponent>().Detach(*this, &DX11DebugPass::OnLineComponentAdded);
	world_->ComponentRemoved<LineComponent>().Detach(*this, &DX11DebugPass::OnLineComponentRemoved);
}

void DX11DebugPass::UpdateLineData(Entity camera) {
	Matrix4f camera_matrix = CameraUtils::GetCameraMatrix(*world_, camera);

	world_->ForEach<TransformComponent, LineComponent, DX11LineDataComponent>(
	[&, this](auto entity, auto transform_component, auto line_component, auto line_data_component) {
		Matrix4f world = TransformUtils::GetGlobalWorldMatrix(*world_, *relationeer_, entity);
		Matrix4f world_view_projection = camera_matrix * world;

		LineData line_data{};
		line_data.world_view_projection = world_view_projection.Transpose();
		line_data.color = line_component->color;

		line_data_component->Update(&line_data);
	});
}

void DX11DebugPass::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = render_context_->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteAll, GPUComparsionFunction::kLess };
	const DX11GPUDepthState& depth_state = render_context_->GetDepthState(ds_id);

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRasterizerState(rasterizer_state);
	context.SetDepthState(depth_state);
	context.SetVertexShader(vertex_shader_);
	context.SetPixelShader(pixel_shader_);
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kLineStrip);
}

void DX11DebugPass::OnTransformComponentAdded(Entity entity) {
	if (world_->Has<LineComponent>(entity) && !world_->Has<DX11LineDataComponent>(entity)) {
		world_->Add<DX11LineDataComponent>(entity);

		auto line_component = world_->Get<LineComponent>(entity);
		vertex_buffers_.emplace(entity, CreateVertexBuffer(line_component->points));
	}
}

void DX11DebugPass::OnTransformComponentRemoved(Entity entity) {
	if (world_->Has<DX11LineDataComponent>(entity)) {
		world_->Remove<DX11LineDataComponent>(entity);
		vertex_buffers_.erase(entity);
	}
}

void DX11DebugPass::OnLineComponentAdded(Entity entity) {
	if (world_->Has<TransformComponent>(entity) && !world_->Has<DX11LineDataComponent>(entity)) {
		world_->Add<DX11LineDataComponent>(entity);

		auto line_component = world_->Get<LineComponent>(entity);
		vertex_buffers_.emplace(entity, CreateVertexBuffer(line_component->points));
	}
}

void DX11DebugPass::OnLineComponentRemoved(Entity entity) {
	if (world_->Has<DX11LineDataComponent>(entity)) {
		world_->Remove<DX11LineDataComponent>(entity);
		vertex_buffers_.erase(entity);
	}
}

} // namespace aoe