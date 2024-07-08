#include "pch.h"

#include "DX11DirectionalLightPass.h"
#include "DX11RenderDataComponents.h"
#include "DirectionalLightComponent.h"
#include "CameraUtils.h"

namespace aoe {

DX11DirectionalLightPass::DX11DirectionalLightPass()
	: vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DirectionalLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DirectionalLightPass.hlsl"))
{}

void DX11DirectionalLightPass::Initialize(const aoe::ServiceProvider& service_provider) {
	DX11RenderPassBase::Initialize(service_provider);

	InitializeDirectionLightData();
	SubscribeToComponents();
}

void DX11DirectionalLightPass::Terminate() {
	UnsibscribeFromComponents();
}

void DX11DirectionalLightPass::Update() {
	Entity camera = CameraUtils::GetActualCamera(*GetWorld());
	Vector3f camera_position = GetGlobalPosition(camera);
	auto filter = GetWorld()->GetFilter<TransformComponent, DirectionalLightComponent, DX11DirectionalLightDataComponent>();

	for (Entity entity : filter) {
		auto directional_light_component = GetWorld()->Get<DirectionalLightComponent>(entity);
		auto directional_light_data_component = GetWorld()->Get<DX11DirectionalLightDataComponent>(entity);

		DirectionalLightData data{};
		data.view_position = camera_position;
		data.direction = GetGlobalTransform(entity).GetForward();
		data.color = directional_light_component->color;

		directional_light_data_component->Update(&data);
	}
}

void DX11DirectionalLightPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	auto filter = GetWorld()->GetFilter<TransformComponent, DX11DirectionalLightDataComponent>();

	for (Entity entity : filter) {
		auto direction_light_data_component = GetWorld()->Get<DX11DirectionalLightDataComponent>(entity);

		context.SetConstantBuffer(GPUShaderType::kPixel, direction_light_data_component->buffer, 0);
		context.Draw(4);
	}
}

void DX11DirectionalLightPass::InitializeDirectionLightData() {
	auto filter = GetWorld()->GetFilter<DirectionalLightComponent>();

	for (Entity entity : filter) {
		GetWorld()->Add<DX11DirectionalLightDataComponent>(entity);
	}
}

void DX11DirectionalLightPass::SubscribeToComponents() {
	GetWorld()->ComponentAdded<DirectionalLightComponent>().Attach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentAdded);
	GetWorld()->ComponentRemoved<DirectionalLightComponent>().Attach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentRemoved);
}

void DX11DirectionalLightPass::UnsibscribeFromComponents() {
	GetWorld()->ComponentAdded<DirectionalLightComponent>().Detach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentAdded);
	GetWorld()->ComponentRemoved<DirectionalLightComponent>().Detach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentRemoved);
}

void DX11DirectionalLightPass::PrepareRenderContext() {
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

void DX11DirectionalLightPass::OnDirectionLightComponentAdded(Entity entity) {
	GetWorld()->Add<DX11DirectionalLightDataComponent>(entity);
}

void DX11DirectionalLightPass::OnDirectionLightComponentRemoved(Entity entity) {
	GetWorld()->Remove<DX11DirectionalLightDataComponent>(entity);
}

} // namespace aoe