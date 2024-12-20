#include "pch.h"

#include "DX11AmbientLightPassTickSystem.h"
#include "DX11RenderDataComponents.h"
#include "DX11ShaderHelper.h"

namespace aoe {

DX11AmbientLightPassTickSystem::DX11AmbientLightPassTickSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/AmbientLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/AmbientLightPass.hlsl"))
{}

void DX11AmbientLightPassTickSystem::Update(float dt) {
	if (HasCamera()) {
		Render();
	}
}

void DX11AmbientLightPassTickSystem::Render() {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	PrepareRenderContext();

	for (Entity entity : FilterEntities<AmbientLightDataComponent>()) {
		auto ambient_light_data_component = GetComponent<AmbientLightDataComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kPixel, ambient_light_data_component->light_data.buffer, 0);
		context.Draw(4);
	}
}

void DX11AmbientLightPassTickSystem::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = GetRenderContext()->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteNone, GPUComparsionFunction::kLess };
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
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleStrip);
}

} // namespace aoe