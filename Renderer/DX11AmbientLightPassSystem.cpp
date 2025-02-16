#include "pch.h"

#include "DX11AmbientLightPassSystem.h"
#include "DX11ShaderHelper.h"
#include "DX11AmbientLightComponent.h"

namespace aoe {

DX11AmbientLightPassSystem::DX11AmbientLightPassSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/AmbientLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/AmbientLightPass.hlsl"))
{}

void DX11AmbientLightPassSystem::Update(float dt) {
	if (HasCamera()) {
		Render();
	}
}

void DX11AmbientLightPassSystem::Render() {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	PrepareRenderContext();

	for (Entity entity : FilterEntities<DX11AmbientLightComponent>()) {
		auto ambient_light_component = GetComponent<DX11AmbientLightComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kPixel, ambient_light_component->GetColorData().buffer, 0);
		context.Draw(4);
	}
}

void DX11AmbientLightPassSystem::PrepareRenderContext() {
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