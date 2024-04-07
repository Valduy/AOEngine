#pragma once

#include "../Core/Debug.h"
#include "../Game/IECSSystem.h"
#include "../Game/ServiceProvider.h"
#include "../Graphics/DX11GPUShadersCompiler.h"
#include "../ECS/World.h"

#include "DX11RenderContext.h"
#include "DX11RenderState.h"
#include "RenderComponent.h"

namespace aoe {

class DX11RenderSystem : public IECSSystem {
public:
	DX11RenderSystem(const Window& window, const aoe::ServiceProvider& service_provider)
		: service_provider_(service_provider)
		, root_directory_(Platform::GetExecutableDirectory())
		, render_context_(window)
		, render_state_()
		, geometry_pass_vertex_shader_byte_code_(CreateGeometryPassVertexShaderByteCode(root_directory_))
		, geometry_pass_pixel_shader_byte_code_(CreateGeometryPassPixelShaderByteCode(root_directory_))
		, geometry_pass_vertex_shader_(geometry_pass_vertex_shader_byte_code_)
		, geometry_pass_pixel_shader_(geometry_pass_pixel_shader_byte_code_)
		, defered_render_targets_()
		, output_render_targets_()
		, window_render_targets_()
		, sampler_(CreateSamplerDescription())
		, cull_back_rasterizer_state_(CreateCullBackRasterizerStateDescription())
		, geometry_blend_state_(CreateOpaqueBlendStateDescription())
		, geometry_depth_state_(CreateOpaqueDepthStateDescription())
		, world_(nullptr)
	{}

	void Initialize() override {
		world_ = service_provider_.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");

		InitializeRenderTargets();
	}

	void Terminate() override {}

	void PerTickUpdate(float dt) override {
		GeometryPass();
	}

	void PerFrameUpdate(float dt) override {
		// TODO: update positions and materials
	}

private:
	const ServiceProvider& service_provider_;
	
	std::wstring root_directory_;

	DX11RenderContext render_context_;
	DX11RenderState render_state_;

	DX11GPUByteCode geometry_pass_vertex_shader_byte_code_;
	DX11GPUByteCode geometry_pass_pixel_shader_byte_code_;

	DX11GPUVertexShader geometry_pass_vertex_shader_;
	DX11GPUPixelShader geometry_pass_pixel_shader_;

	DX11GPURenderTargets defered_render_targets_;
	DX11GPURenderTargets output_render_targets_;
	DX11GPURenderTargets window_render_targets_;

	DX11GPUSampler sampler_;

	DX11GPURasterizerState cull_back_rasterizer_state_;

	DX11GPUDepthState geometry_depth_state_;
	DX11GPUBlendState geometry_blend_state_;

	World* world_;
	
	static DX11GPUByteCode CreateGeometryPassVertexShaderByteCode(std::wstring root_directory) {
		GPUShaderProgramDescription shader_program_desc{
			GPUShaderType::kVertex,
			root_directory + L"/Content/GeometryPass.hlsl", // TODO: mb declare shader path as const
			"VertexMain",
		};

		return DX11GPUShadersCompiler::CompileShader(shader_program_desc);
	}

	static DX11GPUByteCode CreateGeometryPassPixelShaderByteCode(std::wstring root_directory) {
		GPUShaderProgramDescription shader_program_desc{
			GPUShaderType::kPixel,
			root_directory + L"/Content/GeometryPass.hlsl",
			"PixelMain",
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

	static GPUBlendStateDescription CreateOpaqueBlendStateDescription() {
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
		blend_state_desc.render_targets[0].render_target_write_mask = 0x0f;

		return blend_state_desc;
	}

	static GPUDepthStateDescription CreateOpaqueDepthStateDescription() {
		return {
			true,
			GPUDepthWriteMask::kWriteAll,
			GPUComparsionFunction::kLess,
		};
	}

	void InitializeRenderTargets() {
		const DX11GPUTextureView** defered_render_target_views = defered_render_targets_.render_target_views;
		defered_render_target_views[0] = &render_context_.GetDiffuseTextureView();
		defered_render_target_views[1] = &render_context_.GetNormalTextureView();
		defered_render_target_views[2] = &render_context_.GetPositionTextureView();
		defered_render_targets_.depth_stencil_view = &render_context_.GetDepthBufferView();

		const DX11GPUTextureView** output_render_target_views = output_render_targets_.render_target_views;
		output_render_target_views[0] = &render_context_.GetOutputTextureView();
		output_render_targets_.depth_stencil_view = &render_context_.GetDepthBufferView();

		const DX11GPUTextureView** window_render_target_views = window_render_targets_.render_target_views;
		window_render_target_views[0] = &render_context_.GetRenderTargetView();
		window_render_targets_.depth_stencil_view = &render_context_.GetDepthBufferView();
	}

	void GeometryPass() {
		DX11GPUContext context = DX11GPUDevice::Instance().GetContext();
		context.SetRasterizerState(cull_back_rasterizer_state_);
		context.SetBlendState(geometry_blend_state_, 0xffffffff);
		context.SetDepthState(geometry_depth_state_);
		context.SetVertexShader(geometry_pass_vertex_shader_);
		context.SetPixelShader(geometry_pass_pixel_shader_);
		context.SetSampler(GPUShaderType::kPixel, sampler_, 0);
		context.SetRenderTargets(defered_render_targets_);
		context.SetPrimitiveTopology(GPUPrimitiveTopology::kTriangleList);

		world_->ForEach<RenderComponent>([](auto entity, auto render_component) {
			// TODO: iterate over all mathced components and render
		});
	}
};

} // namespace aoe