#include <windows.h>
#include <math.h>

#include "../Game/Executor.h"
#include "../Application/Application.h"
#include "../Graphics/GPUDevice.h"
#include "../Graphics/GPUContext.h"
#include "../Graphics/GPUSwapChain.h"
#include "../Graphics/GPUShadersCompiler.h"
#include "../Graphics/GPUTextureData.h"
#include "../Graphics/GPUSampler.h"

#include "xdata.h"

struct Vector { float x, y, z; };
struct Matrix { float m[4][4]; };

Matrix operator*(const Matrix& m1, const Matrix& m2);

struct Constants {
    Matrix transform;
    Matrix projection;
    Vector light_vector;
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
        , texture_data_(ToVector<uint32_t>(kTextureData, ARRAYSIZE(kTextureData)))
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

        aoe::GPUDepthStateDescription depth_state_desc;
        depth_state_desc.is_depth_enabled = true;
        depth_state_desc.write_mask = aoe::GPUDepthWriteMask::kWriteAll;
        depth_state_desc.comparsion_function = aoe::GPUComparsionFunction::kLess;
        depth_state_.Initialize(depth_state_desc);

        aoe::GPURasteriserStateDescription rasterizer_state_desc;
        rasterizer_state_desc.cull_mode = aoe::GPUCullMode::kBack;
        rasterizer_state_desc.fill_mode = aoe::GPUFillMode::kSolid;
        rasterized_state_.Initialize(rasterizer_state_desc);

        auto vertex_buffer_desc = aoe::GPUBufferDescription::CreateVertex<Vertex>(kVertexData, ARRAYSIZE(kVertexData));
        vertex_buffer_.Initialize(vertex_buffer_desc);

        auto index_buffer_desc = aoe::GPUBufferDescription::CreateIndex<int32_t>(kIndexData, ARRAYSIZE(kIndexData));
        index_buffer_.Initialize(index_buffer_desc);

        auto constant_buffer_desc = aoe::GPUBufferDescription::CreateConstant<Constants>(aoe::GPUResourceUsage::kDynamic);
        constant_buffer_.Initialize(constant_buffer_desc);

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
        float background_color[4] = { 1.01f, 0.01f, 0.01f, 1.0f };
        
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

        Matrix rotate_x = { 1, 0, 0, 0, 0, static_cast<float>(cos(model_rotation_.x)), -static_cast<float>(sin(model_rotation_.x)), 0, 0, static_cast<float>(sin(model_rotation_.x)), static_cast<float>(cos(model_rotation_.x)), 0, 0, 0, 0, 1 };
        Matrix rotate_y = { static_cast<float>(cos(model_rotation_.y)), 0, static_cast<float>(sin(model_rotation_.y)), 0, 0, 1, 0, 0, -static_cast<float>(sin(model_rotation_.y)), 0, static_cast<float>(cos(model_rotation_.y)), 0, 0, 0, 0, 1 };
        Matrix rotate_z = { static_cast<float>(cos(model_rotation_.z)), -static_cast<float>(sin(model_rotation_.z)), 0, 0, static_cast<float>(sin(model_rotation_.z)), static_cast<float>(cos(model_rotation_.z)), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
        Matrix scale = { model_scale_.x, 0, 0, 0, 0, model_scale_.y, 0, 0, 0, 0, model_scale_.z, 0, 0, 0, 0, 1 };
        Matrix translate = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, model_translation_.x, model_translation_.y, model_translation_.z, 1 };

        model_rotation_.x += 0.005f;
        model_rotation_.y += 0.009f;
        model_rotation_.z += 0.001f;

        auto context = device_.GetContext();
        context.ClearState();

        Constants constants{};
        constants.transform = rotate_x * rotate_y * rotate_z * scale * translate;
        constants.projection = { 2 * n / w, 0, 0, 0, 0, 2 * n / h, 0, 0, 0, 0, f / (f - n), 1, 0, 0, n * f / (n - f), 0 };
        constants.light_vector = { 1.0f, -1.0f, 1.0f };
        context.UpdateBuffer<Constants>(constant_buffer_, &constants, 1);

        context.ClearRenderTarget(swap_chain_.GetRenderTargetView(), background_color);
        context.ClearDepth(depth_stencil_buffer_.GetDepthStencilView(), 1.0f);

        context.SetVertexBuffer(vertex_buffer_);
        context.SetIndexBuffer(index_buffer_);

        context.SetPrimitiveTopology(aoe::GPUPrimitiveTopology::kTriangleList);
        context.SetVertexShader(vertex_shader_);
        context.SetConstantBuffer(aoe::GPUShaderType::kVertex, constant_buffer_);

        context.SetViewport(viewport);
        context.SetRasterizerState(rasterized_state_);

        context.SetPixelShader(pixel_shader_);
        context.SetShaderResource(aoe::GPUShaderType::kPixel, texture_, 0);
        context.SetSampler(aoe::GPUShaderType::kPixel, sampler_);

        context.SetRenderTarget(swap_chain_.GetRenderTargetView(), depth_stencil_buffer_.GetDepthStencilView());
        context.SetDepthState(depth_state_);

        context.DrawIndexed(index_buffer_.GetDescription().GetElementsCount());

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

    aoe::GPUBuffer vertex_buffer_;
    aoe::GPUBuffer index_buffer_;
    aoe::GPUBuffer constant_buffer_;

    aoe::GPUVertexShader vertex_shader_;
    aoe::GPUPixelShader pixel_shader_;

    aoe::GPUSampler sampler_;

    aoe::GPUTextureData<uint32_t> texture_data_;
    aoe::GPUTexture2D texture_;

    Vector model_rotation_ = { 0.0f, 0.0f, 0.0f };
    Vector model_scale_ = { 1.0f, 1.0f, 1.0f };
    Vector model_translation_ = { 0.0f, 0.0f, 4.0f };
    
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

Matrix operator*(const Matrix& m1, const Matrix& m2)
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