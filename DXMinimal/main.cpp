#include <windows.h>
#include <math.h>

#include "../Application/Application.h"
#include "../Graphics/GPUDevice.h"
#include "../Graphics/GPUContext.h"
#include "../Graphics/GPUSwapChain.h"
#include "../Graphics/GPUShadersCompiler.h"
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
        , texture_(device_)
    {}

    void Initialize() override {
        bool result = false;

        device_.Initialize();
        swap_chain_.Initialize();

        aoe::GPUTexture2DDescription depth_stencil_desc;
        depth_stencil_desc.width = window_.GetWidth();
        depth_stencil_desc.height = window_.GetHeight();
        depth_stencil_desc.pixel_format = aoe::GPUPixelFormat::kD24_Unorm_S8_Uint;
        depth_stencil_desc.texture_flags = aoe::GPUTextureFlags::kDepthStencil;
        result = depth_stencil_buffer_.Initialize(depth_stencil_desc);
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

        auto vertex_buffer_desc = aoe::GPUBufferDescription::CreateVertex<Vertex>(kVertexData, ARRAYSIZE(kVertexData));
        result = vertex_buffer_.Initialize(vertex_buffer_desc);
        AOE_ASSERT(result);

        auto index_buffer_desc = aoe::GPUBufferDescription::CreateIndex<int32_t>(kIndexData, ARRAYSIZE(kIndexData));
        result = index_buffer_.Initialize(index_buffer_desc);
        AOE_ASSERT(result);

        auto constant_buffer_desc = aoe::GPUBufferDescription::CreateConstant<Constants>(aoe::GPUResourceUsage::kDynamic);
        result = constant_buffer_.Initialize(constant_buffer_desc);
        AOE_ASSERT(result);

        aoe::GPUShadersCompiler shader_compiler(device_);

        aoe::DXByteCode vertex_byte_code = shader_compiler.CompileVertexShader(L"shaders.hlsl", "vs_main");
        result = vertex_shader_.Initialize(vertex_byte_code, {
            {aoe::LayoutElementSemantics::kPosition, aoe::GPUPixelFormat::kR32G32B32_Float},
            {aoe::LayoutElementSemantics::kNormal, aoe::GPUPixelFormat::kR32G32B32_Float},
            {aoe::LayoutElementSemantics::kTexcoord, aoe::GPUPixelFormat::kR32G32_Float},
            {aoe::LayoutElementSemantics::kColor, aoe::GPUPixelFormat::kR32G32B32_Float},
        });
        AOE_ASSERT(result);

        aoe::DXByteCode pixel_byte_code = shader_compiler.CompilePixelShader(L"shaders.hlsl", "ps_main");
        result = pixel_shader_.Initialize(pixel_byte_code);
        AOE_ASSERT(result);

        aoe::GPUSamplerDescription sampler_desc;
        result = sampler_.Initialize(sampler_desc);
        AOE_ASSERT(result);

        auto texture_desc = aoe::GPUTexture2DDescription::Create<uint32_t>(
            TEXTURE_WIDTH,
            TEXTURE_HEIGHT,
            aoe::GPUPixelFormat::kR8G8B8A8_Unorm,
            aoe::GPUTextureFlags::kShaderResource,
            kTextureData
        );
        result = texture_.Initialize(texture_desc);
        AOE_ASSERT(result);
    };

    void Terminate() override {
        texture_.Terminate();
        sampler_.Terminate();
        pixel_shader_.Terminate();
        vertex_shader_.Terminate();
        constant_buffer_.Terminate();
        index_buffer_.Terminate();
        vertex_buffer_.Terminate();
        rasterized_state_.Terminate();
        depth_state_.Terminate();
        depth_stencil_buffer_.Terminate();
        swap_chain_.Terminate();
        device_.Terminate();
    };

    void PerTickUpdate(float dt) override {
        float background_color[4] = { 1.01f, 0.01f, 0.01f, 1.0f };
        
        auto context = device_.GetContext();
        context.ClearState();

        context.ClearRenderTarget(swap_chain_.GetRenderTargetView(), background_color);
        context.ClearDepth(depth_stencil_buffer_.GetDepthStencilView(), 1.0f);

        context.SetVertexBuffer(vertex_buffer_);
        context.SetIndexBuffer(index_buffer_);

        context.SetPrimitiveTopology(aoe::GPUPrimitiveTopology::kTriangleList);
        context.SetVertexShader(vertex_shader_);
        context.SetConstantBuffer(aoe::GPUShaderType::kVertex, constant_buffer_);

        context.SetViewport(viewport_);
        context.SetRasterizerState(rasterized_state_);

        context.SetPixelShader(pixel_shader_);
        context.SetShaderResource(aoe::GPUShaderType::kPixel, texture_, 0);
        context.SetSampler(aoe::GPUShaderType::kPixel, sampler_);

        context.SetRenderTarget(swap_chain_.GetRenderTargetView(), depth_stencil_buffer_.GetDepthStencilView());
        context.SetDepthState(depth_state_);

        context.DrawIndexed(index_buffer_.GetElementsCount());

        swap_chain_.Present();
    };

    void PerFrameUpdate(float dt) override {
        viewport_ = GetViewport();

        const float aspect = viewport_.width / viewport_.height;
        auto context = device_.GetContext();

        Matrix rotate_x = RotationX(model_rotation_.x);
        Matrix rotate_y = RotationY(model_rotation_.y);
        Matrix rotate_z = RotationZ(model_rotation_.z);
        Matrix scale = Scale(model_scale_);
        Matrix translate = Translate(model_translation_);
        Matrix transform = rotate_x * rotate_y * rotate_z * scale * translate;

        model_rotation_.x += 0.5f * dt;
        model_rotation_.y += 0.9f * dt;
        model_rotation_.z += 0.1f * dt;

        Constants constants{};
        constants.transform = transform;
        constants.projection = Projection(aspect, 1.0f, 9.0f);
        constants.light_vector = { 1.0f, -1.0f, 1.0f };
        context.UpdateBuffer<Constants>(constant_buffer_, &constants, 1);
    };

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

    aoe::GPUTexture2D texture_;

    aoe::Viewport viewport_;

    Vector model_rotation_ = { 0.0f, 0.0f, 0.0f };
    Vector model_scale_ = { 1.0f, 1.0f, 1.0f };
    Vector model_translation_ = { 0.0f, 0.0f, 4.0f };

    static Matrix RotationX(float angle) {
        Matrix result;
        const float cosa = static_cast<float>(cos(angle));
        const float sina = static_cast<float>(sin(angle));
        
        result.m[0][0] = 1;
        result.m[0][1] = 0;
        result.m[0][2] = 0;
        result.m[0][3] = 0;

        result.m[1][0] = 0;
        result.m[1][1] = cosa;
        result.m[1][2] = -sina;
        result.m[1][3] = 0;

        result.m[2][0] = 0;
        result.m[2][1] = sina;
        result.m[2][2] = cosa;
        result.m[2][3] = 0;

        result.m[3][0] = 0;
        result.m[3][1] = 0;
        result.m[3][2] = 0;
        result.m[3][3] = 1;

        return result;
    }

    static Matrix RotationY(float angle) {
        Matrix result;
        const float cosa = static_cast<float>(cos(angle));
        const float sina = static_cast<float>(sin(angle));

        result.m[0][0] = cosa;
        result.m[0][1] = 0;
        result.m[0][2] = sina;
        result.m[0][3] = 0;

        result.m[1][0] = 0;
        result.m[1][1] = 1;
        result.m[1][2] = 0;
        result.m[1][3] = 0;

        result.m[2][0] = -sina;
        result.m[2][1] = 0;
        result.m[2][2] = cosa;
        result.m[2][3] = 0;

        result.m[3][0] = 0;
        result.m[3][1] = 0;
        result.m[3][2] = 0;
        result.m[3][3] = 1;

        return result;
    }

    static Matrix RotationZ(float angle) {
        Matrix result;
        const float cosa = static_cast<float>(cos(angle));
        const float sina = static_cast<float>(sin(angle));

        result.m[0][0] = cosa;
        result.m[0][1] = -sina;
        result.m[0][2] = 0;
        result.m[0][3] = 0;

        result.m[1][0] = sina;
        result.m[1][1] = cosa;
        result.m[1][2] = 0;
        result.m[1][3] = 0;

        result.m[2][0] = 0;
        result.m[2][1] = 0;
        result.m[2][2] = 1;
        result.m[2][3] = 0;

        result.m[3][0] = 0;
        result.m[3][1] = 0;
        result.m[3][2] = 0;
        result.m[3][3] = 1;

        return result;
    }

    const Matrix Scale(const Vector& scale) {
        Matrix result;

        result.m[0][0] = scale.x;
        result.m[0][1] = 0;
        result.m[0][2] = 0;
        result.m[0][3] = 0;

        result.m[1][0] = 0;
        result.m[1][1] = scale.y;
        result.m[1][2] = 0;
        result.m[1][3] = 0;

        result.m[2][0] = 0;
        result.m[2][1] = 0;
        result.m[2][2] = scale.z;
        result.m[2][3] = 0;

        result.m[3][0] = 0;
        result.m[3][1] = 0;
        result.m[3][2] = 0;
        result.m[3][3] = 1;

        return result;
    }

    static Matrix Translate(const Vector& translation) {
        Matrix result;

        result.m[0][0] = 1;
        result.m[0][1] = 0;
        result.m[0][2] = 0;
        result.m[0][3] = 0;

        result.m[1][0] = 0;
        result.m[1][1] = 1;
        result.m[1][2] = 0;
        result.m[1][3] = 0;

        result.m[2][0] = 0;
        result.m[2][1] = 0;
        result.m[2][2] = 1;
        result.m[2][3] = 0;

        result.m[3][0] = translation.x;
        result.m[3][1] = translation.y;
        result.m[3][2] = translation.z;
        result.m[3][3] = 1;

        return result;
    }

    static Matrix Projection(float aspect, float near_plain, float far_plain) {
        Matrix result;

        result.m[0][0] = 2 * near_plain / aspect;
        result.m[0][1] = 0;
        result.m[0][2] = 0;
        result.m[0][3] = 0;

        result.m[1][0] = 0;
        result.m[1][1] = 2 * near_plain;
        result.m[1][2] = 0;
        result.m[1][3] = 0;

        result.m[2][0] = 0;
        result.m[2][1] = 0;
        result.m[2][2] = far_plain / (far_plain - near_plain);
        result.m[2][3] = 1;

        result.m[3][0] = 0;
        result.m[3][1] = 0;
        result.m[3][2] = near_plain * far_plain / (near_plain - far_plain);
        result.m[3][3] = 0;

        return result;
    }

    aoe::Viewport GetViewport() {
        const float width = static_cast<float>(window_.GetWidth());
        const float height = static_cast<float>(window_.GetHeight());

        return { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    }
};

int main() {
    aoe::Application application(L"Game", 800, 600);

    MyGame game(application.GetWindow());
    application.Start(game);

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