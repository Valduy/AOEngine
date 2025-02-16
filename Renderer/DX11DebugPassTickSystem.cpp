#include "pch.h"

#include "DX11DebugPassTickSystem.h"
#include "DX11RenderDataComponents.h"
#include "DX11ShaderHelper.h"
#include "LineComponent.h"

namespace aoe {

DX11DebugPassTickSystem::DX11DebugPassTickSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DebugPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DebugPass.hlsl"))
	, camera_data_()
{}

void DX11DebugPassTickSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		Render(camera);
	}
}

void DX11DebugPassTickSystem::Render(Entity camera) {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	PrepareRenderContext();

	Matrix4f view_projection = GetCameraMatrix(camera);
	Matrix4f view_projection_t = view_projection.Transpose();
	camera_data_.Update(&view_projection_t);

	context.SetConstantBuffer(GPUShaderType::kVertex, camera_data_.buffer, 0);

	for (Entity entity : FilterEntities<TransformComponent, LineComponent>()) {
		auto line_component = GetComponent<LineComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kVertex, line_component->GetTransformData().buffer, 1);
		context.SetConstantBuffer(GPUShaderType::kVertex, line_component->GetColorData().buffer, 2);

		for (const DX11GPUBuffer& vertex_buffer : line_component->GetLineResources()) {
			context.SetVertexBuffer(vertex_buffer);
			context.Draw(vertex_buffer.GetElementsCount());
		}
	}
}

void DX11DebugPassTickSystem::PrepareRenderContext() {
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