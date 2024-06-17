#include "pch.h"

#include "../Graphics/DX11GPUContext.h"

#include "DX11AmbientLightPass.h"
#include "DX11RenderDataComponents.h"
#include "AmbientLightComponent.h"

namespace aoe {

DX11AmbientLightPass::DX11AmbientLightPass()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/AmbientLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/AmbientLightPass.hlsl"))
{}

void DX11AmbientLightPass::Initialize(const aoe::ServiceProvider& service_provider) {
	DX11RenderPassBase::Initialize(service_provider);
	
	InitializeAmbientLightData();
	SubscribeToComponents();
}

void DX11AmbientLightPass::Terminate() {
	UnsibscribeFromComponents();
}

inline void DX11AmbientLightPass::Update() {
	auto filter = GetWorld()->GetFilter<AmbientLightComponent, DX11AmbientLightDataComponent>();

	for (Entity entity : filter) {
		auto ambient_light_component = GetWorld()->Get<AmbientLightComponent>(entity);
		auto ambient_light_data_component = GetWorld()->Get<DX11AmbientLightDataComponent>(entity);

		AmbientLightData data{};
		data.color = ambient_light_component->color;
		data.intensity = ambient_light_component->intensity;

		ambient_light_data_component->Update(&data);
	}
}

void DX11AmbientLightPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	auto filter = GetWorld()->GetFilter<DX11AmbientLightDataComponent>();

	for (Entity entity : filter) {
		auto ambient_light_data_component = GetWorld()->Get<DX11AmbientLightDataComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kPixel, ambient_light_data_component->buffer, 0);
		context.Draw(4);
	}
}

void DX11AmbientLightPass::InitializeAmbientLightData() {
	auto filter = GetWorld()->GetFilter<AmbientLightComponent>();

	for (Entity entity : filter) {
		GetWorld()->Add<DX11AmbientLightDataComponent>(entity);
	}
}

void DX11AmbientLightPass::SubscribeToComponents() {
	GetWorld()->ComponentAdded<AmbientLightComponent>().Attach(*this, &DX11AmbientLightPass::OnAmbientLightComponentAdded);
	GetWorld()->ComponentRemoved<AmbientLightComponent>().Attach(*this, &DX11AmbientLightPass::OnAmbientLightComponentRemoved);
}

void DX11AmbientLightPass::UnsibscribeFromComponents() {
	GetWorld()->ComponentAdded<AmbientLightComponent>().Detach(*this, &DX11AmbientLightPass::OnAmbientLightComponentAdded);
	GetWorld()->ComponentRemoved<AmbientLightComponent>().Detach(*this, &DX11AmbientLightPass::OnAmbientLightComponentRemoved);
}

void DX11AmbientLightPass::PrepareRenderContext() {
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

void DX11AmbientLightPass::OnAmbientLightComponentAdded(Entity entity) {
	GetWorld()->Add<DX11AmbientLightDataComponent>(entity);
}

void DX11AmbientLightPass::OnAmbientLightComponentRemoved(Entity entity) {
	GetWorld()->Remove<DX11AmbientLightDataComponent>(entity);
}

} // namespace aoe