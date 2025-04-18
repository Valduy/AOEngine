#include "pch.h"

#include "../Game/TransformChangedComponent.h"

#include "DX11PointLightPassSystem.h"
#include "DX11ShaderHelper.h"
#include "DX11PointLightComponent.h"

namespace aoe {

DX11PointLightPassSystem::DX11PointLightPassSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/PointLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/PointLightPass.hlsl"))
	, model_manager_(nullptr)
	, sphere_id(DX11ModelManager::kDefault)
{}

void DX11PointLightPassSystem::Initialize(const ServiceProvider& service_provider) {
	DX11RenderPassSystemBase::Initialize(service_provider);

	model_manager_ = service_provider.TryGetService<DX11ModelManager>();
	AOE_ASSERT_MSG(model_manager_ != nullptr, "There is no DX11ModelManager service.");

	sphere_id = model_manager_->Load(L"Content/Sphere.fbx");
}

void DX11PointLightPassSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		UpdateRenderData();
		Render(camera);
	}
}

void DX11PointLightPassSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<TransformComponent, TransformChangedComponent, DX11PointLightComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(entity);
		auto point_light_component = GetComponent<DX11PointLightComponent>(entity);

		const Matrix4f world = GetGlobalWorldMatrix(entity);
		const Vector3f position = GetGlobalPosition(entity);

		PointLightTransformData data{};
		data.world = world.Transpose();
		data.position = position;

		point_light_component->transform_data_.Update(&data);
	}
}

void DX11PointLightPassSystem::Render(Entity camera) {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	const DX11ModelResources& sphere_resources = model_manager_->GetModelResources(sphere_id);
	PrepareRenderContext();

	auto camera_component = GetComponent<DX11CameraComponent>(camera);
	context.SetConstantBuffer(GPUShaderType::kVertex, camera_component->GetCameraData().buffer, 0);
	context.SetConstantBuffer(GPUShaderType::kPixel, camera_component->GetTransformData().buffer, 2);

	for (Entity entity : FilterEntities<DX11PointLightComponent>()) {
		auto point_light_data_component = GetComponent<DX11PointLightComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kVertex, point_light_data_component->GetTransformData().buffer, 1);
		context.SetConstantBuffer(GPUShaderType::kPixel, point_light_data_component->GetColorData().buffer, 3);

		for (const DX11MeshResources& mesh_resource : sphere_resources.meshes_resources) {
			context.SetVertexBuffer(mesh_resource.vertex_buffer);
			context.SetIndexBuffer(mesh_resource.index_buffer);
			context.DrawIndexed(mesh_resource.index_buffer.GetElementsCount());
		}
	}
}

void DX11PointLightPassSystem::PrepareRenderContext() {
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

} // namespace aoe