#include "pch.h"

#include "DX11DirectionalLightPassTickSystem.h"
#include "DX11ShaderHelper.h"
#include "DirectionalLightComponent.h"

namespace aoe {

DX11DirectionalLightPassTickSystem::DX11DirectionalLightPassTickSystem()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DirectionalLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DirectionalLightPass.hlsl"))
{}

void DX11DirectionalLightPassTickSystem::Update(float dt) {
	if (HasCamera()) {
		Entity camera = GetActualCamera();
		Render(camera);
	}
}

void DX11DirectionalLightPassTickSystem::Render(Entity camera) {
	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	PrepareRenderContext();

	Vector3fData data{};
	data.value = GetGlobalPosition(camera);
	camera_data_.Update(&data);

	context.SetConstantBuffer(GPUShaderType::kPixel, camera_data_.buffer, 0);

	for (Entity entity : FilterEntities<TransformComponent, DirectionalLightComponent>()) {
		auto direction_light_component = GetComponent<DirectionalLightComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kPixel, direction_light_component->GetTransformData().buffer, 1);
		context.SetConstantBuffer(GPUShaderType::kPixel, direction_light_component->GetColorData().buffer, 2);
		context.Draw(4);
	}
}

void DX11DirectionalLightPassTickSystem::PrepareRenderContext() {
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