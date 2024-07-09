#include "pch.h"

#include "DX11GeometryPass.h"
#include "DX11RenderDataComponents.h"
#include "RenderComponent.h"
#include "CameraUtils.h"

namespace aoe {

DX11GeometryPass::DX11GeometryPass()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/GeometryPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/GeometryPass.hlsl"))
	, sampler_(CreateSamplerDescription())
	, blend_state_(CreateBlendStateDescription())
	, model_manager_(nullptr)
	, texture_manager_(nullptr)
{}

void DX11GeometryPass::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassBase::Initialize(service_provider);

	model_manager_ = service_provider.GetService<DX11ModelManager>();
	AOE_ASSERT_MSG(model_manager_ != nullptr, "There is no DX11ModelManager service.");

	texture_manager_ = service_provider.GetService<DX11TextureManager>();
	AOE_ASSERT_MSG(texture_manager_ != nullptr, "There is no DX11TextureManager service.");

	InitializeGeometryData();
	SubscribeToComponents();
}

void DX11GeometryPass::Terminate() {
	UnsibscribeFromComponents();
}

void DX11GeometryPass::Update() {
	Entity camera = CameraUtils::GetActualCamera(*GetWorld());
	UpdateGeometryData(camera);
}

void DX11GeometryPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	auto filter = GetWorld()->GetFilter<RenderComponent, GeometryDataComponent>();

	for (Entity entity : filter) {
		auto render_component = GetWorld()->Get<RenderComponent>(entity);
		auto geometry_data_component = GetWorld()->Get<GeometryDataComponent>(entity);

		const DX11ModelResources& model_resources = model_manager_->GetModelResources(render_component->model_id);
		const DX11GPUTexture2D& texture_resources = texture_manager_->GetTextureResources(render_component->texture_id);

		context.SetConstantBuffer(GPUShaderType::kVertex, geometry_data_component->transform_data.buffer, 0);
		context.SetConstantBuffer(GPUShaderType::kPixel, geometry_data_component->material_data.buffer, 1);
		context.SetShaderResource(GPUShaderType::kPixel, texture_resources.GetTextureView(), 0);

		for (const DX11MeshResources& mesh_resource : model_resources.meshes_resources) {
			context.SetVertexBuffer(mesh_resource.vertex_buffer);
			context.SetIndexBuffer(mesh_resource.index_buffer);
			context.DrawIndexed(mesh_resource.index_buffer.GetElementsCount());
		}
	}
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

void DX11GeometryPass::InitializeGeometryData() {
	auto filter = GetWorld()->GetFilter<TransformComponent, RenderComponent>();

	for (Entity entity : filter) {
		GetWorld()->Add<GeometryDataComponent>(entity);
	}
}

void DX11GeometryPass::SubscribeToComponents() {
	GetWorld()->ComponentAdded<TransformComponent>().Attach(*this, &DX11GeometryPass::OnTransformComponentAdded);
	GetWorld()->ComponentAdded<RenderComponent>().Attach(*this, &DX11GeometryPass::OnRenderComponentAdded);

	GetWorld()->ComponentRemoved<RenderComponent>().Attach(*this, &DX11GeometryPass::OnComponentRemoved);
}

void DX11GeometryPass::UnsibscribeFromComponents() {
	GetWorld()->ComponentAdded<TransformComponent>().Detach(*this, &DX11GeometryPass::OnTransformComponentAdded);
	GetWorld()->ComponentAdded<RenderComponent>().Detach(*this, &DX11GeometryPass::OnRenderComponentAdded);

	GetWorld()->ComponentRemoved<RenderComponent>().Detach(*this, &DX11GeometryPass::OnComponentRemoved);
}

void DX11GeometryPass::UpdateGeometryData(Entity camera) {
	Matrix4f camera_matrix = GetCameraMatrix(camera);
	auto filter = GetWorld()->GetFilter<TransformComponent, RenderComponent, GeometryDataComponent>();

	for (Entity entity : filter) {
		auto transform_component = GetWorld()->Get<TransformComponent>(entity);
		auto render_component = GetWorld()->Get<RenderComponent>(entity);
		auto geometry_data_component = GetWorld()->Get<GeometryDataComponent>(entity);

		Matrix4f world = GetGlobalWorldMatrix(entity);
		Matrix4f world_view_projection = camera_matrix * world;

		TransformData transform_data{};
		transform_data.world = world.Transpose();
		transform_data.world_view_projection = world_view_projection.Transpose();
		transform_data.inverse_transpose_world = world.Inverse();

		MaterialData material_data{};
		material_data.diffuse = render_component->material.diffuse;
		material_data.specular = render_component->material.specular;
		material_data.shininess = render_component->material.shininess;

		geometry_data_component->transform_data.Update(&transform_data);
		geometry_data_component->material_data.Update(&material_data);
	}
}

void DX11GeometryPass::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = GetRenderContext()->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteAll, GPUComparsionFunction::kLess };
	const DX11GPUDepthState& depth_state = GetRenderContext()->GetDepthState(ds_id);

	DX11GPURenderTargets render_targets;
	render_targets.render_target_views[0] = &GetRenderContext()->GetDiffuseTextureView();
	render_targets.render_target_views[1] = &GetRenderContext()->GetSpecularTextureView();
	render_targets.render_target_views[2] = &GetRenderContext()->GetNormalTextureView();
	render_targets.render_target_views[3] = &GetRenderContext()->GetPositionTextureView();
	render_targets.depth_stencil_view = &GetRenderContext()->GetDepthBufferView();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRenderTargets(render_targets);
	context.SetRasterizerState(rasterizer_state);
	context.SetBlendState(blend_state_, 0xffffffff);
	context.SetDepthState(depth_state);
	context.SetVertexShader(vertex_shader_);
	context.SetPixelShader(pixel_shader_);
	context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleList);
}

void DX11GeometryPass::OnTransformComponentAdded(Entity entity) {
	if (GetWorld()->Has<RenderComponent>(entity)) {
		GetWorld()->Add<GeometryDataComponent>(entity);
	}
}

void DX11GeometryPass::OnRenderComponentAdded(Entity entity) {
	if (GetWorld()->Has<TransformComponent>(entity)) {
		GetWorld()->Add<GeometryDataComponent>(entity);
	}
}

void DX11GeometryPass::OnComponentRemoved(Entity entity) {
	GetWorld()->Remove<GeometryDataComponent>(entity);
}

} // namespace aoe