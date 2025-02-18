#include "pch.h"

#include "../Game/TransformChangedComponent.h"

#include "DX11DebugPassSystem.h"
#include "DX11RenderData.h"
#include "DX11ShaderHelper.h"
#include "DX11LineComponent.h"

namespace aoe {

DX11DebugPassSystem::DX11DebugPassSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DebugPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DebugPass.hlsl"))
	, camera_data_()
{}

void DX11DebugPassSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		UpdateRenderData();
		Render(camera);
	}
}

void DX11DebugPassSystem::UpdateRenderData() {
	for (Entity entity : FilterEntities<TransformComponent, TransformChangedComponent, DX11LineComponent>()) {
		auto transform_component = GetComponent<TransformComponent>(entity);
		auto line_component = GetComponent<DX11LineComponent>(entity);

		const Matrix4f& world = transform_component->GetGlobalWorldMatrix();
		const Matrix4f world_t = world.Transpose();

		line_component->transform_data_.Update(&world_t);
	}
}

void DX11DebugPassSystem::Render(Entity camera) {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	PrepareRenderContext();

	Matrix4f view_projection = GetCameraMatrix(camera);
	Matrix4f view_projection_t = view_projection.Transpose();
	camera_data_.Update(&view_projection_t);

	context.SetConstantBuffer(GPUShaderType::kVertex, camera_data_.buffer, 0);

	for (Entity entity : FilterEntities<TransformComponent, DX11LineComponent>()) {
		auto line_component = GetComponent<DX11LineComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kVertex, line_component->GetTransformData().buffer, 1);
		context.SetConstantBuffer(GPUShaderType::kVertex, line_component->GetColorData().buffer, 2);

		for (const DX11GPUBuffer& vertex_buffer : line_component->GetLineResources()) {
			context.SetVertexBuffer(vertex_buffer);
			context.Draw(vertex_buffer.GetElementsCount());
		}
	}
}

void DX11DebugPassSystem::PrepareRenderContext() {
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

} // namespace aoe