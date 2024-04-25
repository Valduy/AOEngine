#pragma once

#include "../Core/Debug.h"
#include "../Game/IECSSystem.h"
#include "../Game/ServiceProvider.h"
#include "../Graphics/DX11GPUShadersCompiler.h"
#include "../ECS/World.h"
#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"

#include "DX11RenderContext.h"
#include "DX11RenderState.h"
#include "DX11RenderDataComponents.h"
#include "RenderComponent.h"
#include "AmbientLightComponent.h"
#include "CameraUtils.h"

namespace aoe {

class DX11RenderSystem : public IECSSystem {
public:
	DX11RenderSystem(const Window& window, const aoe::ServiceProvider& service_provider)
		: service_provider_(service_provider)
		, root_directory_(Platform::GetExecutableDirectory())
		, render_context_(window)
		, render_state_()
		, geometry_pass_vertex_shader_byte_code_(CreateVertexShaderByteCode(root_directory_ + L"/Content/GeometryPass.hlsl"))
		, geometry_pass_pixel_shader_byte_code_(CreatePixelShaderByteCode(root_directory_ + L"/Content/GeometryPass.hlsl"))
		, ambient_light_vertex_shader_byte_code_(CreateVertexShaderByteCode(root_directory_ + L"/Content/AmbientLightPass.hlsl"))
		, ambient_light_pixel_shader_byte_code_(CreatePixelShaderByteCode(root_directory_ + L"/Content/AmbientLightPass.hlsl"))
		, direction_light_vertex_shader_byte_code_(CreateVertexShaderByteCode(root_directory_ + L"/Content/DirectionLightPass.hlsl"))
		, direction_light_pixel_shader_byte_code_(CreatePixelShaderByteCode(root_directory_ + L"/Content/DirectionLightPass.hlsl"))
		, tone_pass_vertex_shader_byte_code_(CreateVertexShaderByteCode(root_directory_ + L"/Content/TonePass.hlsl"))
		, tone_pass_pixel_shader_byte_code_(CreatePixelShaderByteCode(root_directory_ + L"/Content/TonePass.hlsl"))
		, geometry_pass_vertex_shader_(geometry_pass_vertex_shader_byte_code_)
		, geometry_pass_pixel_shader_(geometry_pass_pixel_shader_byte_code_)
		, ambient_light_vertex_shader_(ambient_light_vertex_shader_byte_code_)
		, ambient_light_pixel_shader_(ambient_light_pixel_shader_byte_code_)
		, direction_light_vertex_shader_(direction_light_vertex_shader_byte_code_)
		, direction_light_pixel_shader_(direction_light_pixel_shader_byte_code_)
		, tone_pass_vertex_shader_(tone_pass_vertex_shader_byte_code_)
		, tone_pass_pixel_shader_(tone_pass_pixel_shader_byte_code_)
		, geometry_render_targets_()
		, light_render_targets_()
		, window_render_targets_()
		, sampler_(CreateSamplerDescription())
		, cull_back_rasterizer_state_(CreateCullBackRasterizerStateDescription())
		, write_all_less_depth_state_(CreateWriteAllLessDepthStateDescription())
		, none_less_depth_state_(CreateNoneLessDepthStateDescription())
		, none_greater_depth_state_(CreateNoneGreaterDepthStateDescription())
		, no_depth_state_(CreateNoDepthStateDescription())
		, geometry_blend_state_(CreateGeometryBlendStateDescription())
		, light_blend_state_(CreateLightBlendStateDescription())
		, world_(nullptr)
		, model_manager_(nullptr)
		, texture_manager_(nullptr)
	{}

	void Initialize() override {
		world_ = service_provider_.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		model_manager_ = service_provider_.GetService<DX11ModelManager>();
		AOE_ASSERT_MSG(model_manager_ != nullptr, "There is no DX11ModelManager service.");

		texture_manager_ = service_provider_.GetService<DX11TextureManager>();
		AOE_ASSERT_MSG(texture_manager_ != nullptr, "There is no DX11TextureManager service.");

		InitializeRenderTargets();
	}

	void Terminate() override {}

	void PerTickUpdate(float dt) override {
		PrepareContext();

		// TODO: may be use it for viewport setup
		Entity camera = CameraUtils::GetActualCamera(*world_);

		if (camera.IsNull()) {
			return;
		}

		GeometryPass();
		LightPass();
		TonePass();
		PresentFrame();
	}

	void PerFrameUpdate(float dt) override {}

private:
	const ServiceProvider& service_provider_;
	
	std::wstring root_directory_;

	DX11RenderContext render_context_;
	DX11RenderState render_state_;

	DX11GPUByteCode geometry_pass_vertex_shader_byte_code_;
	DX11GPUByteCode geometry_pass_pixel_shader_byte_code_;

	DX11GPUByteCode ambient_light_vertex_shader_byte_code_;
	DX11GPUByteCode ambient_light_pixel_shader_byte_code_;

	DX11GPUByteCode direction_light_vertex_shader_byte_code_;
	DX11GPUByteCode direction_light_pixel_shader_byte_code_;

	DX11GPUByteCode tone_pass_vertex_shader_byte_code_;
	DX11GPUByteCode tone_pass_pixel_shader_byte_code_;

	DX11GPUVertexShader geometry_pass_vertex_shader_;
	DX11GPUPixelShader geometry_pass_pixel_shader_;

	DX11GPUVertexShader ambient_light_vertex_shader_;
	DX11GPUPixelShader ambient_light_pixel_shader_;

	DX11GPUVertexShader direction_light_vertex_shader_;
	DX11GPUPixelShader direction_light_pixel_shader_;

	DX11GPUVertexShader tone_pass_vertex_shader_;
	DX11GPUPixelShader tone_pass_pixel_shader_;

	DX11GPURenderTargets geometry_render_targets_;
	DX11GPURenderTargets light_render_targets_;
	DX11GPURenderTargets window_render_targets_;

	DX11GPUSampler sampler_;

	DX11GPURasterizerState cull_back_rasterizer_state_;

	DX11GPUDepthState write_all_less_depth_state_;
	DX11GPUDepthState none_less_depth_state_;
	DX11GPUDepthState none_greater_depth_state_;
	DX11GPUDepthState no_depth_state_;

	DX11GPUBlendState geometry_blend_state_;
	DX11GPUBlendState light_blend_state_;

	World* world_;
	DX11ModelManager* model_manager_;
	DX11TextureManager* texture_manager_;

	static DX11GPUByteCode CreateVertexShaderByteCode(std::wstring path) {
		GPUShaderProgramDescription shader_program_desc{
			GPUShaderType::kVertex, path, "VertexMain",
		};

		return DX11GPUShadersCompiler::CompileShader(shader_program_desc);
	}

	static DX11GPUByteCode CreatePixelShaderByteCode(std::wstring path) {
		GPUShaderProgramDescription shader_program_desc{
			GPUShaderType::kPixel, path, "PixelMain",
		};

		return DX11GPUShadersCompiler::CompileShader(shader_program_desc);
	}

	static GPUSamplerDescription CreateSamplerDescription() {
		return {};
	}

	static GPURasterizerStateDescription CreateCullBackRasterizerStateDescription() {
		return {
			GPUCullMode::kBack,
			GPUFillMode::kSolid,
		};
	}

	static GPUBlendStateDescription CreateGeometryBlendStateDescription() {
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

	static GPUBlendStateDescription CreateLightBlendStateDescription() {
		GPUBlendStateDescription blend_state_desc{};
		blend_state_desc.is_alpha_to_coverage_enable = false;
		blend_state_desc.is_independent_blend_enable = false;
		blend_state_desc.render_targets[0].is_blend_enable = true;
		blend_state_desc.render_targets[0].source_blend = GPUBlend::kOne;
		blend_state_desc.render_targets[0].destination_blend = GPUBlend::kOne;
		blend_state_desc.render_targets[0].blend_operation = GPUBlendOperation::kAdd;
		blend_state_desc.render_targets[0].source_blend_alpha = GPUBlend::kOne;
		blend_state_desc.render_targets[0].destination_blend_alpha = GPUBlend::kOne;
		blend_state_desc.render_targets[0].blend_operation_alpha = GPUBlendOperation::kAdd;
		blend_state_desc.render_targets[0].color_write_mask = GPUColorWriteMask::kAll;

		return blend_state_desc;
	}

	static GPUDepthStateDescription CreateWriteAllLessDepthStateDescription() {
		return {
			true,
			GPUDepthWriteMask::kWriteAll,
			GPUComparsionFunction::kLess,
		};
	}

	static GPUDepthStateDescription CreateNoneLessDepthStateDescription() {
		return {
			true,
			GPUDepthWriteMask::kWriteNone,
			GPUComparsionFunction::kLess,
		};
	}

	static GPUDepthStateDescription CreateNoneGreaterDepthStateDescription() {
		return {
			true,
			GPUDepthWriteMask::kWriteNone,
			GPUComparsionFunction::kGreater,
		};
	}

	static GPUDepthStateDescription CreateNoDepthStateDescription() {
		return {
			false,
			GPUDepthWriteMask::kWriteNone,
			GPUComparsionFunction::kNever,
		};
	}

	void PrepareContext() {
		const float background_color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };

		aoe::DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
		context.ClearRenderTarget(render_context_.GetRenderTargetView(), background_color);
		context.ClearRenderTarget(render_context_.GetDiffuseTextureView(), background_color);
		context.ClearRenderTarget(render_context_.GetNormalTextureView(), background_color);
		context.ClearRenderTarget(render_context_.GetPositionTextureView(), background_color);
		context.ClearRenderTarget(render_context_.GetAccumulatorTextureView(), background_color);
		context.ClearDepth(render_context_.GetDepthBufferView(), 1.0f);
		context.ClearState();
		context.SetViewport(render_context_.GetViewport());
	}

	void InitializeRenderTargets() {
		const DX11GPUTextureView** defered_render_target_views = geometry_render_targets_.render_target_views;
		defered_render_target_views[0] = &render_context_.GetDiffuseTextureView();
		defered_render_target_views[1] = &render_context_.GetSpecularTextureView();
		defered_render_target_views[2] = &render_context_.GetNormalTextureView();
		defered_render_target_views[3] = &render_context_.GetPositionTextureView();
		geometry_render_targets_.depth_stencil_view = &render_context_.GetDepthBufferView();

		const DX11GPUTextureView** output_render_target_views = light_render_targets_.render_target_views;
		output_render_target_views[0] = &render_context_.GetAccumulatorTextureView();
		light_render_targets_.depth_stencil_view = &render_context_.GetDepthBufferView();

		const DX11GPUTextureView** window_render_target_views = window_render_targets_.render_target_views;
		window_render_target_views[0] = &render_context_.GetRenderTargetView();
		window_render_targets_.depth_stencil_view = &render_context_.GetDepthBufferView();
	}

	void GeometryPass() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRenderTargets(geometry_render_targets_);
		context.SetRasterizerState(cull_back_rasterizer_state_);
		context.SetBlendState(geometry_blend_state_, 0xffffffff);
		context.SetDepthState(write_all_less_depth_state_);
		context.SetVertexShader(geometry_pass_vertex_shader_);
		context.SetPixelShader(geometry_pass_pixel_shader_);
		context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
		context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleList);

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

	void LightPass() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRenderTargets(light_render_targets_);
		context.SetBlendState(light_blend_state_, 0xffffffff);

		AmbientLightPass();
		DirectionLightPass();
		PointLightPass();
	}

	void AmbientLightPass() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRasterizerState(cull_back_rasterizer_state_);
		context.SetDepthState(none_less_depth_state_);
		context.SetVertexShader(ambient_light_vertex_shader_);
		context.SetPixelShader(ambient_light_pixel_shader_);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetDiffuseTextureView(), 0);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetSpecularTextureView(), 1);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetNormalTextureView(), 2);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetPositionTextureView(), 3);
		context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleStrip);

		world_->ForEach<DX11AmbientLightDataComponent>(
		[&, this](auto entity, auto ambient_light_data_component) {
			context.SetConstantBuffer(GPUShaderType::kPixel, ambient_light_data_component->buffer, 0);
			context.Draw(4);
		});
	}

	void DirectionLightPass() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRasterizerState(cull_back_rasterizer_state_);
		context.SetDepthState(none_less_depth_state_);
		context.SetVertexShader(direction_light_vertex_shader_);
		context.SetPixelShader(direction_light_pixel_shader_);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetDiffuseTextureView(), 0);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetSpecularTextureView(), 1);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetNormalTextureView(), 2);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetPositionTextureView(), 3);
		context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleStrip);

		world_->ForEach<DX11DirectionLightDataComponent>(
		[&, this](auto entity, auto direction_light_data_component) {
			context.SetConstantBuffer(GPUShaderType::kPixel, direction_light_data_component->buffer, 0);
			context.Draw(4);
		});
	}

	void PointLightPass() {
		// TODO: implement point light.
	}

	void TonePass() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRasterizerState(cull_back_rasterizer_state_);
		context.SetBlendState(geometry_blend_state_, 0xffffffff);
		context.SetDepthState(no_depth_state_);
		context.SetVertexShader(tone_pass_vertex_shader_);
		context.SetPixelShader(tone_pass_pixel_shader_);
		context.SetRenderTargets(window_render_targets_);
		context.SetShaderResource(GPUShaderType::kPixel, render_context_.GetAccumulatorTextureView(), 0);
		context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
		context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleStrip);
		context.Draw(4);
	}

	void PresentFrame() {
		render_context_.PresentFrame();
	}
};

} // namespace aoe