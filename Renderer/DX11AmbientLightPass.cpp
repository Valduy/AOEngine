#include "pch.h"

#include "DX11AmbientLightPass.h"

namespace aoe {

DX11AmbientLightPass::DX11AmbientLightPass(const aoe::ServiceProvider& service_provider)
	: service_provider_(service_provider)
	, vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/AmbientLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/AmbientLightPass.hlsl"))
	, world_(nullptr)
	, render_context_(nullptr)
{}

void DX11AmbientLightPass::Initialize() {
	world_ = service_provider_.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

	render_context_ = service_provider_.GetService<DX11RenderContext>();
	AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");
}

void DX11AmbientLightPass::Terminate() {}

void DX11AmbientLightPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	world_->ForEach<DX11AmbientLightDataComponent>(
	[&, this](auto entity, auto ambient_light_data_component) {
		context.SetConstantBuffer(GPUShaderType::kPixel, ambient_light_data_component->buffer, 0);
		context.Draw(4);
	});
}

void DX11AmbientLightPass::PrepareRenderContext() {
	DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
	const DX11GPURasterizerState& rasterizer_state = render_context_->GetRasterizerState(rs_id);

	DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteNone, GPUComparsionFunction::kLess };
	const DX11GPUDepthState& depth_state = render_context_->GetDepthState(ds_id);

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	context.SetRasterizerState(rasterizer_state);
	context.SetDepthState(depth_state);
	context.SetVertexShader(vertex_shader_);
	context.SetPixelShader(pixel_shader_);
	context.SetShaderResource(GPUShaderType::kPixel, render_context_->GetDiffuseTextureView(), 0);
	context.SetShaderResource(GPUShaderType::kPixel, render_context_->GetSpecularTextureView(), 1);
	context.SetShaderResource(GPUShaderType::kPixel, render_context_->GetNormalTextureView(), 2);
	context.SetShaderResource(GPUShaderType::kPixel, render_context_->GetPositionTextureView(), 3);
	context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleStrip);
}

} // namespace aoe