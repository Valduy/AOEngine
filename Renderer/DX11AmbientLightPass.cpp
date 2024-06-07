#include "pch.h"

#include "DX11AmbientLightPass.h"
#include "AmbientLightComponent.h"

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

	InitializeAmbientLightData();

	SubscribeToComponents();
}

void DX11AmbientLightPass::Terminate() {
	UnsibscribeFromComponents();
}

inline void DX11AmbientLightPass::Update() {
	world_->ForEach<AmbientLightComponent, DX11AmbientLightDataComponent>(
	[this](auto entity, auto ambient_light_component, auto ambient_light_data_component) {
		AmbientLightData data{};
		data.color = ambient_light_component->color;
		data.intensity = ambient_light_component->intensity;

		ambient_light_data_component->Update(&data);
	});
}

void DX11AmbientLightPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	world_->ForEach<DX11AmbientLightDataComponent>(
	[&, this](auto entity, auto ambient_light_data_component) {
		context.SetConstantBuffer(GPUShaderType::kPixel, ambient_light_data_component->buffer, 0);
		context.Draw(4);
	});
}

void DX11AmbientLightPass::InitializeAmbientLightData() {
	world_->ForEach<AmbientLightComponent>(
	[this](auto entity, auto ambient_light_component) {
		world_->Add<DX11AmbientLightDataComponent>(entity);
	});
}

void DX11AmbientLightPass::SubscribeToComponents() {
	world_->ComponentAdded<AmbientLightComponent>().Attach(*this, &DX11AmbientLightPass::OnAmbientLightComponentAdded);
	world_->ComponentRemoved<AmbientLightComponent>().Attach(*this, &DX11AmbientLightPass::OnAmbientLightComponentRemoved);
}

void DX11AmbientLightPass::UnsibscribeFromComponents() {
	world_->ComponentAdded<AmbientLightComponent>().Detach(*this, &DX11AmbientLightPass::OnAmbientLightComponentAdded);
	world_->ComponentRemoved<AmbientLightComponent>().Detach(*this, &DX11AmbientLightPass::OnAmbientLightComponentRemoved);
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

void DX11AmbientLightPass::OnAmbientLightComponentAdded(Entity entity) {
	world_->Add<DX11AmbientLightDataComponent>(entity);
}

void DX11AmbientLightPass::OnAmbientLightComponentRemoved(Entity entity) {
	world_->Remove<DX11AmbientLightDataComponent>(entity);
}

} // namespace aoe