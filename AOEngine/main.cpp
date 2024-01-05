#include <iostream>

#include "../ECS/World.h"
#include "../Game/Executor.h"
#include "../Application/Application.h"
#include "../Core/Logger.h"
#include "../Core/Debug.h"
#include "../Graphics/IGPUResource.h"
#include "../Graphics/GPUDevice.h"
#include "../Graphics/GPUContext.h"
#include "../Graphics/GPUSwapChain.h"
#include "../Graphics/GPUEnums.h"
#include "../Graphics/GPUBuffers.h"
#include "../Graphics/GPUShadersCompiler.h"
#include "../Graphics/GPUShader.h"

struct Vertex {
	float position[4];
	float color[4];
};

class TestGame : public aoe::IGame {
public:
	TestGame(const aoe::Window& window)
		: window_(window)
		, device_()
		, swap_chain_(device_, window_)
		, depth_stencil_buffer_(device_)
		, depth_state_(device_)
		, rasterized_state_(device_)
		, shader_compiler_(device_)
		, vertex_buffer_(device_)
		, vertex_shader_(device_)
		, pixel_shader_(device_)
	{}

	void Initialize() override {
		bool result = false;
		
		result = device_.Initialize();
		AOE_ASSERT(result);

		result = swap_chain_.Initialize();
		AOE_ASSERT(result);

		result = depth_stencil_buffer_.Initialize(
			window_.GetWidth(),
			window_.GetHeight(),
			aoe::GPUPixelFormat::kD24_Unorm_S8_Uint, 
			aoe::GPUTextureFlags::kDepthStencil);
		AOE_ASSERT(result);

		aoe::GPUDepthStateDescription depth_state_desc;
		depth_state_desc.is_depth_enabled = true;
		depth_state_desc.write_mask = aoe::GPUDepthWriteMask::kWriteAll;
		depth_state_desc.comparsion_function = aoe::GPUComparsionFunction::kLess;
		result = depth_state_.Initialize(depth_state_desc);
		AOE_ASSERT(result);

		aoe::GPURasteriserStateDescription rasterizer_state_desc;
		rasterizer_state_desc.cull_mode = aoe::GPUCullMode::kBack;
		rasterizer_state_desc.fill_mode = aoe::GPUFillMode::kSolid;
		result = rasterized_state_.Initialize(rasterizer_state_desc);
		AOE_ASSERT(result);

		std::vector<Vertex> vertices = {
			{{ 0.0f, -1.0f, 0.5f, 1.0f}, {1.0f, 0.0f, 0.0f, 1.0f}},
			{{-1.0f,  1.0f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f, 1.0f}},
			{{ 1.0f,  1.0f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f, 1.0f}},
		};
		auto vertex_buffer_desc = aoe::GPUBufferDescription::CreateVertex<Vertex>(vertices.data(), vertices.size());
		vertex_buffer_.Initialize(vertex_buffer_desc);
		AOE_ASSERT(result);

		aoe::GPUShadersCompiler shader_compiler(device_);

		aoe::DXByteCode vertex_byte_code = shader_compiler.CompileVertexShader(L"VSSimple.hlsl");
		result = vertex_shader_.Initialize(vertex_byte_code, { 
			{aoe::LayoutElementSemantics::kPosition, aoe::GPUPixelFormat::kR32G32B32A32_Float }, 
			{aoe::LayoutElementSemantics::kColor, aoe::GPUPixelFormat::kR32G32B32A32_Float },
		});
		AOE_ASSERT(result);

		aoe::DXByteCode pixel_byte_code = shader_compiler.CompilePixelShader(L"PSSimple.hlsl");
		result = pixel_shader_.Initialize(pixel_byte_code);
		AOE_ASSERT(result);
	}
	
	void Terminate() override {
		pixel_shader_.Terminate();
		vertex_shader_.Terminate();
		vertex_buffer_.Terminate();
		rasterized_state_.Terminate();
		depth_stencil_buffer_.Terminate();
		depth_state_.Terminate();
		swap_chain_.Terminate();
		device_.Terminate();
	}
	
	void PerTickUpdate(float dt) override {
		perTickSum += dt;

		aoe::GPUContext context = device_.GetContext();
		context.ClearState();

		aoe::Viewport viewport;
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = window_.GetWidth();
		viewport.height = window_.GetHeight();
		viewport.min_depth = 0.0f;
		viewport.max_depth = 1.0f;

		const float color[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		aoe::GPURenderTargetView render_target_veiw = swap_chain_.GetRenderTargetView();
		context.ClearRenderTarget(render_target_veiw, color);

		aoe::GPUDepthStencilView depth_stencil_view = depth_stencil_buffer_.GetDepthStencilView();
		context.ClearDepth(depth_stencil_view, 1.0f);

		context.SetRasterizerState(rasterized_state_);
		context.SetDepthState(depth_state_);
		context.SetViewport(viewport);
		context.SetRenderTarget(render_target_veiw, depth_stencil_view);

		context.SetPrimitiveTopology(aoe::GPUPrimitiveTopology::kTriangleList);
		context.SetVertexShader(vertex_shader_);
		context.SetPixelShader(pixel_shader_);
		context.SetVertexBuffer(vertex_buffer_);
		context.Draw(vertex_buffer_.GetDescription().GetElementsCount());

		swap_chain_.Present();
	}

	void PerFrameUpdate(float dt) override {
		perFrameSum += dt;

		if (perFrameSum >= 1.0f) {
			AOE_LOG_INFO("Another one second");
			AOE_LOG_INFO("PerTickSum: {}", perTickSum);
			AOE_LOG_INFO("PerFrameSum: {}", perFrameSum);

			perTickSum = 0;
			perFrameSum = 0;
		}
	}

private:
	const aoe::Window& window_;

	aoe::GPUDevice device_;
	aoe::GPUSwapChain swap_chain_;
	aoe::GPUTexture2D depth_stencil_buffer_;
	aoe::GPUDepthState depth_state_;
	aoe::GPURasterizerState rasterized_state_;
	aoe::GPUShadersCompiler shader_compiler_;

	aoe::GPUBuffer vertex_buffer_;

	aoe::GPUVertexShader vertex_shader_;
	aoe::GPUPixelShader pixel_shader_;

	float perTickSum = 0;
	float perFrameSum = 0;
};

int main() {
	aoe::Application application(L"Game", 800, 600);

	TestGame game(application.GetWindow());

	aoe::Executor executor(game);
	application.SetExecutor(&executor);
	application.Start();

	return 0;
}