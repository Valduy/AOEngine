#include "pch.h"

#include "DX11PointLightPass.h"
#include "DX11RenderDataComponents.h"
#include "PointLightComponent.h"
#include "CameraUtils.h"

namespace aoe {

DX11PointLightPass::DX11PointLightPass()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/PointLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/PointLightPass.hlsl"))
	, model_manager_(nullptr)
	, sphere_id(DX11ModelManager::kDefault)
{}

void DX11PointLightPass::Initialize(const aoe::ServiceProvider& service_provider) {
	DX11RenderPassBase::Initialize(service_provider);

	model_manager_ = service_provider.GetService<DX11ModelManager>();
	AOE_ASSERT_MSG(model_manager_ != nullptr, "There is no DX11ModelManager service.");

	sphere_id = model_manager_->Load(L"Content/Sphere.fbx");

	InitializePointLightData();
	SubscribeToComponents();
}

void DX11PointLightPass::Terminate() {
	UnsibscribeFromComponents();
}

void DX11PointLightPass::Update() {
	Entity camera = GetActualCamera();
	Vector3f camera_position = GetGlobalPosition(camera);
	Matrix4f camera_matrix = GetCameraMatrix(camera);
	auto filter = GetWorld()->GetFilter<TransformComponent, PointLightComponent, PointLightDataComponent>();

	for (Entity entity : filter) {
		auto transform_component = GetWorld()->Get<TransformComponent>(entity);
		auto point_light_component = GetWorld()->Get<PointLightComponent>(entity);
		auto point_light_data_component = GetWorld()->Get<PointLightDataComponent>(entity);

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

void DX11PointLightPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	const DX11ModelResources& sphere_resources = model_manager_->GetModelResources(sphere_id);
	auto filter = GetWorld()->GetFilter<PointLightDataComponent>();

	for (Entity entity : filter) {
		auto point_light_data_component = GetWorld()->Get<PointLightDataComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kVertex, point_light_data_component->transform_data.buffer, 0);
		context.SetConstantBuffer(GPUShaderType::kPixel, point_light_data_component->light_data.buffer, 1);

		for (const DX11MeshResources& mesh_resource : sphere_resources.meshes_resources) {
			context.SetVertexBuffer(mesh_resource.vertex_buffer);
			context.SetIndexBuffer(mesh_resource.index_buffer);
			context.DrawIndexed(mesh_resource.index_buffer.GetElementsCount());
		}
	}
}

void DX11PointLightPass::InitializePointLightData() {
	auto filter = GetWorld()->GetFilter<TransformComponent, PointLightComponent>();

	for (Entity entity : filter) {
		GetWorld()->Add<PointLightDataComponent>(entity);
	}
}

void DX11PointLightPass::SubscribeToComponents() {
	GetWorld()->ComponentAdded<TransformComponent>().Attach(*this, &DX11PointLightPass::OnTransformComponentAdded);
	GetWorld()->ComponentAdded<PointLightComponent>().Attach(*this, &DX11PointLightPass::OnPointLightComponentAdded);

	GetWorld()->ComponentRemoved<PointLightComponent>().Attach(*this, &DX11PointLightPass::OnComponentRemoved);
}

void DX11PointLightPass::UnsibscribeFromComponents() {
	GetWorld()->ComponentAdded<TransformComponent>().Detach(*this, &DX11PointLightPass::OnTransformComponentAdded);
	GetWorld()->ComponentAdded<PointLightComponent>().Detach(*this, &DX11PointLightPass::OnPointLightComponentAdded);

	GetWorld()->ComponentRemoved<PointLightComponent>().Detach(*this, &DX11PointLightPass::OnComponentRemoved);
}

void DX11PointLightPass::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kFront, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = GetRenderContext()->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteNone, GPUComparsionFunction::kGreater };
	const DX11GPUDepthState& depth_state = GetRenderContext()->GetDepthState(ds_id);

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRasterizerState(rasterizer_state);
	context.SetDepthState(depth_state);
	context.SetVertexShader(vertex_shader_);
	context.SetPixelShader(pixel_shader_);
	context.SetShaderResource(GPUShaderType::kPixel, GetRenderContext()->GetDiffuseTextureView(), 0);
	context.SetShaderResource(GPUShaderType::kPixel, GetRenderContext()->GetSpecularTextureView(), 1);
	context.SetShaderResource(GPUShaderType::kPixel, GetRenderContext()->GetNormalTextureView(), 2);
	context.SetShaderResource(GPUShaderType::kPixel, GetRenderContext()->GetPositionTextureView(), 3);
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleList);
}

void DX11PointLightPass::OnTransformComponentAdded(Entity entity) {
	if (GetWorld()->Has<PointLightComponent>(entity)) {
		GetWorld()->Add<PointLightDataComponent>(entity);
	}
}

void DX11PointLightPass::OnPointLightComponentAdded(Entity entity) {
	if (GetWorld()->Has<TransformComponent>(entity)) {
		GetWorld()->Add<PointLightDataComponent>(entity);
	}
}

void DX11PointLightPass::OnComponentRemoved(Entity entity) {
	GetWorld()->Remove<PointLightDataComponent>(entity);
}

} // namespace aoe