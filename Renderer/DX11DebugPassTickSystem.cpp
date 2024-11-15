#include "pch.h"

#include "DX11DebugPassTickSystem.h"
#include "DX11RenderDataComponents.h"
#include "DX11LineResourcesComponent.h"
#include "DX11ShaderHelper.h"
#include "LineComponent.h"

namespace aoe {

DX11DebugPassTickSystem::DX11DebugPassTickSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DebugPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DebugPass.hlsl"))
{}

void DX11DebugPassTickSystem::Update(float dt) {
	if (HasCamera()) {
		Render();
	}
}

void DX11DebugPassTickSystem::Render() {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	PrepareRenderContext();

	for (Entity entity : FilterEntities<LineComponent, LineDataComponent, DX11LineResourcesComponent>()) {
		auto line_data_component = GetComponent<LineDataComponent>(entity);
		auto line_resources_component = GetComponent<DX11LineResourcesComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kVertex, line_data_component->line_data.buffer);

		for (const DX11GPUBuffer& vertex_buffer : line_resources_component->GetLineResources()) {
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