#include "pch.h"

#include "DX11GeometryPassSystem.h"
#include "DX11RenderData.h"
#include "DX11BufferModels.h"
#include "DX11ShaderHelper.h"
#include "DX11RenderComponent.h"

namespace aoe {

DX11GeometryPassSystem::DX11GeometryPassSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/GeometryPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/GeometryPass.hlsl"))
	, sampler_(CreateSamplerDescription())
	, blend_state_(CreateBlendStateDescription())
	, camera_data_()
	, model_manager_(nullptr)
	, texture_manager_(nullptr)
{}

void DX11GeometryPassSystem::Initialize(const ServiceProvider & service_provider) {
	DX11RenderPassSystemBase::Initialize(service_provider);

	model_manager_ = service_provider.TryGetService<DX11ModelManager>();
	AOE_ASSERT_MSG(model_manager_ != nullptr, "There is no DX11ModelManager service.");

	texture_manager_ = service_provider.TryGetService<DX11TextureManager>();
	AOE_ASSERT_MSG(texture_manager_ != nullptr, "There is no DX11TextureManager service.");
}

void DX11GeometryPassSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		UpdateRenderData();
		Render(camera);
	}
}

GPUSamplerDescription DX11GeometryPassSystem::CreateSamplerDescription() {
	return {};
}

GPUBlendStateDescription DX11GeometryPassSystem::CreateBlendStateDescription() {
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

void DX11GeometryPassSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<TransformComponent, DX11RenderComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(entity);
		auto render_component = GetComponent<DX11RenderComponent>(entity);

		Matrix4f world = GetGlobalWorldMatrix(entity);

		TransformData transform_data{};
		transform_data.world = world.Transpose();
		transform_data.world_it = world.Inverse();

		render_component->transform_data_.Update(&transform_data);
	}
}

void DX11GeometryPassSystem::Render(Entity camera) {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	PrepareRenderContext();

	Matrix4f view_projection = GetCameraMatrix(camera);
	Matrix4f view_projection_t = view_projection.Transpose();
	camera_data_.Update(&view_projection_t);

	context.SetConstantBuffer(GPUShaderType::kVertex, camera_data_.buffer, 0);

	for (Entity entity : FilterEntities<TransformComponent, DX11RenderComponent>()) {
		auto render_component = GetComponent<DX11RenderComponent>(entity);
		
		const DX11ModelResources& model_resources = model_manager_->GetModelResources(render_component->GetModelId());
		const DX11GPUTexture2D& texture_resources = texture_manager_->GetTextureResources(render_component->GetTextureId());

		context.SetConstantBuffer(GPUShaderType::kVertex, render_component->GetTransformData().buffer, 1);
		context.SetConstantBuffer(GPUShaderType::kPixel, render_component->GetMaterialData().buffer, 2);
		context.SetShaderResource(GPUShaderType::kPixel, texture_resources.GetTextureView(), 0);

		for (const DX11MeshResources& mesh_resource : model_resources.meshes_resources) {
			context.SetVertexBuffer(mesh_resource.vertex_buffer);
			context.SetIndexBuffer(mesh_resource.index_buffer);
			context.DrawIndexed(mesh_resource.index_buffer.GetElementsCount());
		}
	}
}

void DX11GeometryPassSystem::PrepareRenderContext() {
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

} // namespace aoe