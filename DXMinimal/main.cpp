#pragma comment(lib, "user32")
#pragma comment(lib, "d3d11")
#pragma comment(lib, "d3dcompiler")

///////////////////////////////////////////////////////////////////////////////////////////////////

#include <windows.h>
#include <d3d11_1.h>
#include <d3dcompiler.h>

#include <math.h>  // sin, cos
#include "xdata.h" // 3d model

#include "../Game/Executor.h"
#include "../Application/Application.h"
#include "../Graphics/GPUDevice.h"
#include "../Graphics/GPUContext.h"
#include "../Graphics/GPUSwapChain.h"
#include "../Graphics/GPUShadersCompiler.h"
#include "../Graphics/GPUTextureData.h"
#include "../Graphics/GPUSampler.h"

struct float3 { float x, y, z; };
struct matrix { float m[4][4]; };

matrix operator*(const matrix& m1, const matrix& m2);

struct Constants {
    matrix Transform;
    matrix Projection;
    float3 LightVector;
    float dummy;
};

class MyGame : public aoe::IGame {
public:
    MyGame(const aoe::Window& window)
        : window_(window)
        , device_()
        , swap_chain_(device_, window)
        , depth_stencil_buffer_(device_)
        , depth_state_(device_)
        , rasterized_state_(device_)
        , vertex_buffer_(device_)
        , index_buffer_(device_)
        , constant_buffer_(device_)
        , vertex_shader_(device_)
        , pixel_shader_(device_)
        , sampler_(device_)
        , texture_data_(ToVector<uint32_t>(TextureData, ARRAYSIZE(TextureData)))
        , texture_(device_)
    {}

    void Initialize() override {
        device_.Initialize();
        swap_chain_.Initialize();

        depth_stencil_buffer_.Initialize(
            window_.GetWidth(),
            window_.GetHeight(),
            aoe::GPUPixelFormat::kD24_Unorm_S8_Uint,
            aoe::GPUTextureFlags::kDepthStencil);

        depth_state_.Initialize(
            true,
            aoe::GPUDepthWriteMask::kWriteAll,
            aoe::GPUComparsionFunc::kLess);

        rasterized_state_.Initialize(aoe::GPUCullMode::kBack, aoe::GPUFillMode::kSolid);

        vertex_buffer_.Initialize(ToVector<Vertex>(VertexData, ARRAYSIZE(VertexData)));
        index_buffer_.Initialize(ToVector<int32_t>(IndexData, ARRAYSIZE(IndexData)));
        constant_buffer_.Initialize(aoe::GPUResourceUsage::kDynamic);

        auto device = device_.GetNative();

        aoe::GPUShadersCompiler shader_compiler(device_);

        aoe::DXByteCode vertex_byte_code = shader_compiler.CompileVertexShader(L"shaders.hlsl", "vs_main");
        vertex_shader_.Initialize(vertex_byte_code, {
            {aoe::LayoutElementSemantics::kPosition, aoe::GPUPixelFormat::kR32G32B32_Float},
            {aoe::LayoutElementSemantics::kNormal, aoe::GPUPixelFormat::kR32G32B32_Float},
            {aoe::LayoutElementSemantics::kTexcoord, aoe::GPUPixelFormat::kR32G32_Float},
            {aoe::LayoutElementSemantics::kColor, aoe::GPUPixelFormat::kR32G32B32_Float},
        });

        aoe::DXByteCode pixel_byte_code = shader_compiler.CompilePixelShader(L"shaders.hlsl", "ps_main");
        pixel_shader_.Initialize(pixel_byte_code);

        aoe::GPUSamplerDescription sampler_desc;
        sampler_.Initialize(sampler_desc);

        texture_.Initialize(
            TEXTURE_WIDTH,
            TEXTURE_HEIGHT,
            aoe::GPUPixelFormat::kR8G8B8A8_Unorm,
            aoe::GPUTextureFlags::kShaderResource,
            &texture_data_);
    };

    void Terminate() override {};

    void PerTickUpdate(float dt) override {
        FLOAT backgroundColor[4] = { 1.01f, 0.01f, 0.01f, 1.0f };

        UINT stride = 11 * sizeof(float); // vertex size (11 floats: float3 position, float3 normal, float2 texcoord, float3 color)
        UINT offset = 0;

        aoe::Viewport viewport = {
            0.0f,
            0.0f,
            static_cast<float>(window_.GetWidth()),
            static_cast<float>(window_.GetHeight()),
            0.0f,
            1.0f
        };

        float w = viewport.width / viewport.height; // width (aspect ratio)
        float h = 1.0f;                             // height
        float n = 1.0f;                             // near
        float f = 9.0f;                             // far

        matrix rotateX = { 1, 0, 0, 0, 0, static_cast<float>(cos(modelRotation.x)), -static_cast<float>(sin(modelRotation.x)), 0, 0, static_cast<float>(sin(modelRotation.x)), static_cast<float>(cos(modelRotation.x)), 0, 0, 0, 0, 1 };
        matrix rotateY = { static_cast<float>(cos(modelRotation.y)), 0, static_cast<float>(sin(modelRotation.y)), 0, 0, 1, 0, 0, -static_cast<float>(sin(modelRotation.y)), 0, static_cast<float>(cos(modelRotation.y)), 0, 0, 0, 0, 1 };
        matrix rotateZ = { static_cast<float>(cos(modelRotation.z)), -static_cast<float>(sin(modelRotation.z)), 0, 0, static_cast<float>(sin(modelRotation.z)), static_cast<float>(cos(modelRotation.z)), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        matrix scale = { modelScale.x, 0, 0, 0, 0, modelScale.y, 0, 0, 0, 0, modelScale.z, 0, 0, 0, 0, 1 };
        matrix translate = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, modelTranslation.x, modelTranslation.y, modelTranslation.z, 1 };

        modelRotation.x += 0.005f;
        modelRotation.y += 0.009f;
        modelRotation.z += 0.001f;

        auto context = device_.GetContext();
        auto deviceContext = device_.GetContext().GetNative();

        context.ClearState();

        Constants constants{};
        constants.Transform = rotateX * rotateY * rotateZ * scale * translate;
        constants.Projection = { 2 * n / w, 0, 0, 0, 0, 2 * n / h, 0, 0, 0, 0, f / (f - n), 1, 0, 0, n * f / (n - f), 0 };
        constants.LightVector = { 1.0f, -1.0f, 1.0f };
        context.UpdateBuffer<Constants>(constant_buffer_, constants);

        context.ClearRenderTarget(swap_chain_.GetRenderTargetView(), backgroundColor);
        context.ClearDepth(depth_stencil_buffer_.GetDepthStencilView(), 1.0f);

        context.SetVertexBuffer(vertex_buffer_);
        context.SetIndexBuffer(index_buffer_);

        context.SetPrimitiveTopology(aoe::GPUPrimitiveTopology::kTriangleList);
        context.SetVertexShader(vertex_shader_);
        context.SetConstantBuffer(aoe::GPUShaderType::kVertex, constant_buffer_);

        context.SetViewport(viewport);
        context.SetRasterizerState(rasterized_state_);

        context.SetPixelShader(pixel_shader_);
        context.PSSetShaderResource(texture_, 0);
        context.SetSampler(aoe::GPUShaderType::kPixel, sampler_);

        context.SetRenderTarget(swap_chain_.GetRenderTargetView(), depth_stencil_buffer_.GetDepthStencilView());
        context.SetDepthState(depth_state_);

        context.DrawIndexed(index_buffer_.GetSize());

        swap_chain_.Present();
    };

    void PerFrameUpdate(float dt) override {};

private:
    const aoe::Window& window_;

    aoe::GPUDevice device_;
    aoe::GPUSwapChain swap_chain_;
    aoe::GPUTexture2D depth_stencil_buffer_;
    aoe::GPUDepthState depth_state_;
    aoe::GPURasterizerState rasterized_state_;

    aoe::GPUVertexBuffer<Vertex> vertex_buffer_;
    aoe::GPUIndexBuffer index_buffer_;
    aoe::GPUConstantBuffer<Constants> constant_buffer_;

    aoe::GPUVertexShader vertex_shader_;
    aoe::GPUPixelShader pixel_shader_;

    aoe::GPUSampler sampler_;

    aoe::GPUTextureData<uint32_t> texture_data_;
    aoe::GPUTexture2D texture_;

    float3 modelRotation = { 0.0f, 0.0f, 0.0f };
    float3 modelScale = { 1.0f, 1.0f, 1.0f };
    float3 modelTranslation = { 0.0f, 0.0f, 4.0f };
    
    template<typename TVector, typename TArray>
    static std::vector<TVector> ToVector(TArray* array, size_t size) {
        std::vector<TVector> result;
        result.assign(array, array + size);
        return result;
    }
};

int main() {
    aoe::Application application(L"Game", 800, 600);

    MyGame game(application.GetWindow());

    aoe::Executor executor(game);

    application.SetExecutor(&executor);
    application.Start();

    return 0;
}

matrix operator*(const matrix& m1, const matrix& m2)
{
    return
    {
        m1.m[0][0] * m2.m[0][0] + m1.m[0][1] * m2.m[1][0] + m1.m[0][2] * m2.m[2][0] + m1.m[0][3] * m2.m[3][0],
        m1.m[0][0] * m2.m[0][1] + m1.m[0][1] * m2.m[1][1] + m1.m[0][2] * m2.m[2][1] + m1.m[0][3] * m2.m[3][1],
        m1.m[0][0] * m2.m[0][2] + m1.m[0][1] * m2.m[1][2] + m1.m[0][2] * m2.m[2][2] + m1.m[0][3] * m2.m[3][2],
        m1.m[0][0] * m2.m[0][3] + m1.m[0][1] * m2.m[1][3] + m1.m[0][2] * m2.m[2][3] + m1.m[0][3] * m2.m[3][3],
        m1.m[1][0] * m2.m[0][0] + m1.m[1][1] * m2.m[1][0] + m1.m[1][2] * m2.m[2][0] + m1.m[1][3] * m2.m[3][0],
        m1.m[1][0] * m2.m[0][1] + m1.m[1][1] * m2.m[1][1] + m1.m[1][2] * m2.m[2][1] + m1.m[1][3] * m2.m[3][1],
        m1.m[1][0] * m2.m[0][2] + m1.m[1][1] * m2.m[1][2] + m1.m[1][2] * m2.m[2][2] + m1.m[1][3] * m2.m[3][2],
        m1.m[1][0] * m2.m[0][3] + m1.m[1][1] * m2.m[1][3] + m1.m[1][2] * m2.m[2][3] + m1.m[1][3] * m2.m[3][3],
        m1.m[2][0] * m2.m[0][0] + m1.m[2][1] * m2.m[1][0] + m1.m[2][2] * m2.m[2][0] + m1.m[2][3] * m2.m[3][0],
        m1.m[2][0] * m2.m[0][1] + m1.m[2][1] * m2.m[1][1] + m1.m[2][2] * m2.m[2][1] + m1.m[2][3] * m2.m[3][1],
        m1.m[2][0] * m2.m[0][2] + m1.m[2][1] * m2.m[1][2] + m1.m[2][2] * m2.m[2][2] + m1.m[2][3] * m2.m[3][2],
        m1.m[2][0] * m2.m[0][3] + m1.m[2][1] * m2.m[1][3] + m1.m[2][2] * m2.m[2][3] + m1.m[2][3] * m2.m[3][3],
        m1.m[3][0] * m2.m[0][0] + m1.m[3][1] * m2.m[1][0] + m1.m[3][2] * m2.m[2][0] + m1.m[3][3] * m2.m[3][0],
        m1.m[3][0] * m2.m[0][1] + m1.m[3][1] * m2.m[1][1] + m1.m[3][2] * m2.m[2][1] + m1.m[3][3] * m2.m[3][1],
        m1.m[3][0] * m2.m[0][2] + m1.m[3][1] * m2.m[1][2] + m1.m[3][2] * m2.m[2][2] + m1.m[3][3] * m2.m[3][2],
        m1.m[3][0] * m2.m[0][3] + m1.m[3][1] * m2.m[1][3] + m1.m[3][2] * m2.m[2][3] + m1.m[3][3] * m2.m[3][3],
    };
}