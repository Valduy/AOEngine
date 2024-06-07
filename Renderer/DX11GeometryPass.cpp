#include "pch.h"

#include "DX11GeometryPass.h"
#include "CameraUtils.h"

namespace aoe {

DX11GeometryPass::DX11GeometryPass(const ServiceProvider& service_provider)
	: service_provider_(service_provider)
	, render_targets_()
	, vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/GeometryPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/GeometryPass.hlsl"))
	, sampler_(CreateSamplerDescription())
	, blend_state_(CreateBlendStateDescription())
	, world_(nullptr)
	, render_context_(nullptr)
	, model_manager_(nullptr)
	, texture_manager_(nullptr)
	, relationeer_(nullptr)
{}

void DX11GeometryPass::Initialize() {
	world_ = service_provider_.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

	render_context_ = service_provider_.GetService<DX11RenderContext>();
	AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

	model_manager_ = service_provider_.GetService<DX11ModelManager>();
	AOE_ASSERT_MSG(model_manager_ != nullptr, "There is no DX11ModelManager service.");

	texture_manager_ = service_provider_.GetService<DX11TextureManager>();
	AOE_ASSERT_MSG(texture_manager_ != nullptr, "There is no DX11TextureManager service.");

	relationeer_ = service_provider_.GetService<Relationeer<TransformComponent>>();
	AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");

	InitializeRenderTargets();
	InitializeGeometryData();
	SubscribeToComponents();
}

void DX11GeometryPass::Terminate() {
	UnsibscribeFromComponents();
}

void DX11GeometryPass::Update() {
	Entity camera = CameraUtils::GetActualCamera(*world_);
	UpdateGeometryData(camera);
}

void DX11GeometryPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	world_->ForEach<RenderComponent, DX11TransformDataComponent, DX11MaterialDataComponent>(
	[&, this](auto entity, auto render_component, auto transform_data_component, auto material_data_component) {
		const DX11ModelResources& model_resources = model_manager_->GetModelResources(render_component->model_id);
		const DX11GPUTexture2D& texture_resources = texture_manager_->GetTextureResources(render_component->texture_id);

		context.SetConstantBuffer(GPUShaderType::kVertex, transform_data_component->buffer, 0);
		context.SetConstantBuffer(GPUShaderType::kPixel, material_data_component->buffer, 1);
		context.SetShaderResource(GPUShaderType::kPixel, texture_resources.GetTextureView(), 0);

		for (const DX11MeshResources& mesh_resource : model_resources.mesh_resources) {
			context.SetVertexBuffer(mesh_resource.vertex_buffer);
			context.SetIndexBuffer(mesh_resource.index_buffer);
			context.DrawIndexed(mesh_resource.index_buffer.GetElementsCount());
		}
	});
}

GPUSamplerDescription DX11GeometryPass::CreateSamplerDescription() {
	return {};
}

GPUBlendStateDescription DX11GeometryPass::CreateBlendStateDescription() {
	GPUBlendStateDescription blend_state_desc{};
	blend_state_desc.is_alpha_to_coverage_enable = false;
	blend_state_desc.is_independent_blend_enable = false;
	blend_state_desc.render_targets[0].is_blend_enable = true;
	blend_state_desc.render_targets[0].source_blend = GPUBlend::kOne;
	blend_state_desc.render_targets[0].destination_blend = GPUBlend::kZero;
	blend_state_desc.render_targets[0].blend_operation = GPUBlendOperation::kAdd;
	blend_state_desc.render_targets[0].source_blend_alpha = GPUBlend::kOne;
	blend_state_desc.render_targets[0].destination_blend_alpha = GPUBlend::kZero;
	blend_state_desc.render_targets[0].blend_operation_alpha = GPUBlendOperation::kAdd;
	blend_state_desc.render_targets[0].color_write_mask = GPUColorWriteMask::kAll;

	return blend_state_desc;
}

void DX11GeometryPass::InitializeRenderTargets() {
	render_targets_.render_target_views[0] = &render_context_->GetDiffuseTextureView();
	render_targets_.render_target_views[1] = &render_context_->GetSpecularTextureView();
	render_targets_.render_target_views[2] = &render_context_->GetNormalTextureView();
	render_targets_.render_target_views[3] = &render_context_->GetPositionTextureView();
	render_targets_.depth_stencil_view = &render_context_->GetDepthBufferView();
}

void DX11GeometryPass::InitializeGeometryData() {
	world_->ForEach<TransformComponent, RenderComponent>(
	[this](auto entity, auto transform_component, auto render_component) {
		world_->Add<DX11TransformDataComponent>(entity);
		world_->Add<DX11MaterialDataComponent>(entity);
	});
}

void DX11GeometryPass::SubscribeToComponents() {
	world_->ComponentAdded<TransformComponent>().Attach(*this, &DX11GeometryPass::OnTransformComponentAdded);
	world_->ComponentRemoved<TransformComponent>().Attach(*this, &DX11GeometryPass::OnTransformComponentRemoved);

	world_->ComponentAdded<RenderComponent>().Attach(*this, &DX11GeometryPass::OnRenderComponentAdded);
	world_->ComponentRemoved<RenderComponent>().Attach(*this, &DX11GeometryPass::OnRenderComponentRemoved);
}

void DX11GeometryPass::UnsibscribeFromComponents() {
	world_->ComponentAdded<TransformComponent>().Detach(*this, &DX11GeometryPass::OnTransformComponentAdded);
	world_->ComponentRemoved<TransformComponent>().Detach(*this, &DX11GeometryPass::OnTransformComponentRemoved);

	world_->ComponentAdded<RenderComponent>().Detach(*this, &DX11GeometryPass::OnRenderComponentAdded);
	world_->ComponentRemoved<RenderComponent>().Detach(*this, &DX11GeometryPass::OnRenderComponentRemoved);
}

void DX11GeometryPass::UpdateGeometryData(Entity camera) {
	Matrix4f camera_matrix = CameraUtils::GetCameraMatrix(*world_, camera);

	world_->ForEach<TransformComponent, RenderComponent, DX11TransformDataComponent, DX11MaterialDataComponent>(
	[&, this](auto entity, auto transform_component, auto render_component, auto transform_data_component, auto render_data_component) {
		Matrix4f world = TransformUtils::GetGlobalWorldMatrix(*world_, *relationeer_, entity);
		Matrix4f world_view_projection = camera_matrix * world;

		TransformData transform_data{};
		transform_data.world = world.Transpose();
		transform_data.world_view_projection = world_view_projection.Transpose();
		transform_data.inverse_transpose_world = world.Inverse();

		MaterialData material_data{};
		material_data.diffuse = render_component->material.diffuse;
		material_data.specular = render_component->material.specular;
		material_data.shininess = render_component->material.shininess;

		transform_data_component->Update(&transform_data);
		render_data_component->Update(&material_data);
	});
}

void DX11GeometryPass::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = render_context_->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteAll, GPUComparsionFunction::kLess };
	const DX11GPUDepthState& depth_state = render_context_->GetDepthState(ds_id);

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRenderTargets(render_targets_);
	context.SetRasterizerState(rasterizer_state);
	context.SetBlendState(blend_state_, 0xffffffff);
	context.SetDepthState(depth_state);
	context.SetVertexShader(vertex_shader_);
	context.SetPixelShader(pixel_shader_);
	context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleList);
}

void DX11GeometryPass::OnTransformComponentAdded(Entity entity) {
	if (world_->Has<TransformComponent>(entity) && !world_->Has<DX11TransformDataComponent>(entity)) {
		world_->Add<DX11TransformDataComponent>(entity);
	}

	world_->Add<DX11MaterialDataComponent>(entity);
}

void DX11GeometryPass::OnTransformComponentRemoved(Entity entity) {
	world_->Remove<DX11TransformDataComponent>(entity);
}

void DX11GeometryPass::OnRenderComponentAdded(Entity entity) {
	if (world_->Has<TransformComponent>(entity) && !world_->Has<DX11TransformDataComponent>(entity)) {
		world_->Add<DX11TransformDataComponent>(entity);
	}

	world_->Add<DX11MaterialDataComponent>(entity);
}

void DX11GeometryPass::OnRenderComponentRemoved(Entity entity) {
	world_->Remove<DX11TransformDataComponent>(entity);
	world_->Remove<DX11MaterialDataComponent>(entity);
}

} // namespace aoe