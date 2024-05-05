#pragma once

#include "../Game/ServiceProvider.h"
#include "../Graphics/DX11GPUContext.h"
#include "../Graphics/DX11GPUShadersCompiler.h"
#include "../ECS/World.h"
#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"

#include "IRenderPass.h"
#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"
#include "RenderComponent.h"
#include "DX11RenderDataComponents.h"

namespace aoe {

class DX11GeometryPass : public IRenderPass {
public:
	DX11GeometryPass(const ServiceProvider& service_provider) 
		: service_provider_(service_provider)
		, render_targets_()
		, vertex_shader_(DX11ShaderHelper::CreateVertexShader(L"Content/GeometryPass.hlsl"))
		, pixel_shader_(DX11ShaderHelper::CreatePixelShader(L"Content/GeometryPass.hlsl"))
		, sampler_(CreateSamplerDescription())
		, blend_state_(CreateBlendStateDescription())
		, world_(nullptr)
		, render_context_(nullptr)
		, model_manager_(nullptr)
		, texture_manager_(nullptr)
	{}

	void Initialize() override {
		world_ = service_provider_.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		render_context_ = service_provider_.GetService<DX11RenderContext>();
		AOE_ASSERT_MSG(render_context_ != nullptr, "There is no DX11RenderContext service.");

		model_manager_ = service_provider_.GetService<DX11ModelManager>();
		AOE_ASSERT_MSG(model_manager_ != nullptr, "There is no DX11ModelManager service.");

		texture_manager_ = service_provider_.GetService<DX11TextureManager>();
		AOE_ASSERT_MSG(texture_manager_ != nullptr, "There is no DX11TextureManager service.");

		InitializeRenderTargets();
	}

	void Render() override {
		PrepareRenderContext();

		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		world_->ForEach<RenderComponent, DX11TransformDataComponent, DX11MaterialDataComponent>(
		[&, this](auto entity, auto render_component, auto transform_data_component, auto material_data_component) {
			const DX11ModelResources& model_resources = model_manager_->GetModelResources(render_component->model_id);
			const DX11GPUTexture2D& texture_resources = texture_manager_->GetTextureResources(render_component->texture_id);

			context.SetConstantBuffer(GPUShaderType::kVertex, transform_data_component->buffer, 0);
			context.SetConstantBuffer(GPUShaderType::kPixel, material_data_component->buffer, 1);
			context.SetShaderResource(GPUShaderType::kPixel, texture_resources.GetTextureView(), 0);

			for (const DX11MeshResources& mesh_resource : model_resources.mesh_resources) {
				context.SetVertexBuffer(mesh_resource.vertex_buffer);
				context.SetIndexBuffer(mesh_resource.index_buffer);
				context.DrawIndexed(mesh_resource.index_buffer.GetElementsCount());
			}
		});
	}

private:
	const ServiceProvider& service_provider_;

	DX11GPURenderTargets render_targets_;
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;

	World* world_;
	DX11RenderContext* render_context_;
	DX11ModelManager* model_manager_;
	DX11TextureManager* texture_manager_;

	static GPUSamplerDescription CreateSamplerDescription() {
		return {};
	}

	static GPUBlendStateDescription CreateBlendStateDescription() {
		GPUBlendStateDescription blend_state_desc{};
		blend_state_desc.is_alpha_to_coverage_enable = false;
		blend_state_desc.is_independent_blend_enable = false;
		blend_state_desc.render_targets[0].is_blend_enable = true;
		blend_state_desc.render_targets[0].source_blend = GPUBlend::kOne;
		blend_state_desc.render_targets[0].destination_blend = GPUBlend::kZero;
		blend_state_desc.render_targets[0].blend_operation = GPUBlendOperation::kAdd;
		blend_state_desc.render_targets[0].source_blend_alpha = GPUBlend::kOne;
		blend_state_desc.render_targets[0].destination_blend_alpha = GPUBlend::kZero;
		blend_state_desc.render_targets[0].blend_operation_alpha = GPUBlendOperation::kAdd;
		blend_state_desc.render_targets[0].color_write_mask = GPUColorWriteMask::kAll;

		return blend_state_desc;
	}

	void InitializeRenderTargets() {
		render_targets_.render_target_views[0] = &render_context_->GetDiffuseTextureView();
		render_targets_.render_target_views[1] = &render_context_->GetSpecularTextureView();
		render_targets_.render_target_views[2] = &render_context_->GetNormalTextureView();
		render_targets_.render_target_views[3] = &render_context_->GetPositionTextureView();
		render_targets_.depth_stencil_view = &render_context_->GetDepthBufferView();
	}

	void PrepareRenderContext() {
		DX11RasterizerStateID rs_id{ GPUCullMode::kBack, GPUFillMode::kSolid };
		const DX11GPURasterizerState& rasterizer_state = render_context_->GetRasterizerState(rs_id);

		DX11DepthStateID ds_id{ true, GPUDepthWriteMask::kWriteAll, GPUComparsionFunction::kLess };
		const DX11GPUDepthState& depth_state = render_context_->GetDepthState(ds_id);

		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRenderTargets(render_targets_);
		context.SetRasterizerState(rasterizer_state);
		context.SetBlendState(blend_state_, 0xffffffff);
		context.SetDepthState(depth_state);
		context.SetVertexShader(vertex_shader_);
		context.SetPixelShader(pixel_shader_);
		context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
		context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleList);
	}
};

} // namespace aoe