#include "pch.h"

#include "DX11DirectionalLightPass.h"
#include "DirectionalLightComponent.h"
#include "CameraUtils.h"

namespace aoe {

DX11DirectionalLightPass::DX11DirectionalLightPass(const ServiceProvider& service_provider)
	: service_provider_(service_provider)
	, vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/DirectionalLightPass.hlsl"))
	, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/DirectionalLightPass.hlsl"))
	, world_(nullptr)
	, render_context_(nullptr)
	, relationeer_(nullptr)
{}

void DX11DirectionalLightPass::Initialize() {
	world_ = service_provider_.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

	render_context_ = service_provider_.GetService<DX11RenderContext>();
	AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

	relationeer_ = service_provider_.GetService<Relationeer<TransformComponent>>();
	AOE_ASSERT_MSG(relationeer_ != nullptr, "There is no Relationeer<TransformComponent> service.");

	InitializeDirectionLightData();

	SubscribeToComponents();
}

void DX11DirectionalLightPass::Terminate() {
	UnsibscribeFromComponents();
}

void DX11DirectionalLightPass::Update() {
	Entity camera = CameraUtils::GetActualCamera(*world_);
	UpdateDirectionLightData(camera);
}

void DX11DirectionalLightPass::Render() {
	PrepareRenderContext();

	DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
	world_->ForEach<DX11DirectionalLightDataComponent>(
	[&, this](auto entity, auto direction_light_data_component) {
		context.SetConstantBuffer(GPUShaderType::kPixel, direction_light_data_component->buffer, 0);
		context.Draw(4);
	});
}

void DX11DirectionalLightPass::InitializeDirectionLightData() {
	world_->ForEach<DirectionalLightComponent>(
	[this](auto entity, auto ambient_light_component) {
		world_->Add<DX11DirectionalLightDataComponent>(entity);
	});
}

void DX11DirectionalLightPass::SubscribeToComponents() {
	world_->ComponentAdded<DirectionalLightComponent>().Attach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentAdded);
	world_->ComponentRemoved<DirectionalLightComponent>().Attach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentRemoved);
}

void DX11DirectionalLightPass::UnsibscribeFromComponents() {
	world_->ComponentAdded<DirectionalLightComponent>().Detach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentAdded);
	world_->ComponentRemoved<DirectionalLightComponent>().Detach(*this, &DX11DirectionalLightPass::OnDirectionLightComponentRemoved);
}

void DX11DirectionalLightPass::PrepareRenderContext() {
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

void DX11DirectionalLightPass::UpdateDirectionLightData(Entity camera) {
	auto transform_component = world_->Get<TransformComponent>(camera);

	world_->ForEach<TransformComponent, DirectionalLightComponent, DX11DirectionalLightDataComponent>(
	[&, this](auto entity, auto transform_component, auto direction_light_component, auto direction_light_data_component) {
		DirectionalLightData data{};
		data.view_position = transform_component->position;
		data.direction = TransformUtils::GetGlobalForward(*world_, *relationeer_, entity);
		data.color = direction_light_component->color;

		direction_light_data_component->Update(&data);
	});
}

void DX11DirectionalLightPass::OnDirectionLightComponentAdded(Entity entity) {
	world_->Add<DX11DirectionalLightDataComponent>(entity);
}

void DX11DirectionalLightPass::OnDirectionLightComponentRemoved(Entity entity) {
	world_->Remove<DX11DirectionalLightDataComponent>(entity);
}

} // namespace aoe