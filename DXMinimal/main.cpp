#include <windows.h>
#include <math.h>

#include "../Application/Application.h"
#include "../Graphics/DX11GPUDevice.h"
#include "../Graphics/DX11GPUContext.h"
#include "../Graphics/DX11GPUSwapChain.h"
#include "../Graphics/DX11GPUShadersCompiler.h"

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
        , swap_chain_(nullptr)
        , depth_stencil_buffer_(nullptr)
        , depth_state_(nullptr)
        , rasterized_state_(nullptr)
        , vertex_buffer_(nullptr)
        , index_buffer_(nullptr)
        , constant_buffer_(nullptr)
        , vertex_shader_(device_, aoe::DX11GPUShadersCompiler::CompileShader({aoe::GPUShaderType::kVertex, L"shaders.hlsl", "vs_main"}))
        , pixel_shader_(device_, aoe::DX11GPUShadersCompiler::CompileShader({ aoe::GPUShaderType::kPixel, L"shaders.hlsl", "ps_main"}))
        , sampler_(nullptr)
        , texture_(nullptr)
    {}

    void Initialize() override {
        aoe::IGPUDevice* device_interface = &device_;

        swap_chain_ = device_.CreateSwapChain(window_);

        const aoe::GPUDepthStateDescription depth_state_desc = {
            true,
            aoe::GPUDepthWriteMask::kWriteAll,
            aoe::GPUComparsionFunction::kLess
        };
        depth_state_ = device_.CreateDepthState(depth_state_desc);

        const aoe::GPURasterizerStateDescription rasterizer_state_desc = {
            aoe::GPUCullMode::kBack,
            aoe::GPUFillMode::kSolid,
        };
        rasterized_state_ = device_.CreateRasterizerState(rasterizer_state_desc);
        
        const aoe::GPUSamplerDescription sampler_desc = {};
        sampler_ = device_.CreateSampler(sampler_desc);

        const aoe::GPUBufferDescription vertex_buffer_desc = {
            aoe::GPUBufferType::kVertexBuffer,
            aoe::GPUResourceUsage::kDefault
        };
        vertex_buffer_ = device_interface->CreateBuffer<Vertex>(vertex_buffer_desc, kVertexData, ARRAYSIZE(kVertexData));

        const aoe::GPUBufferDescription index_buffer_desc = {
            aoe::GPUBufferType::kIndexBuffer,
            aoe::GPUResourceUsage::kDefault
        };
        index_buffer_ = device_interface->CreateBuffer<int32_t>(index_buffer_desc, kIndexData, ARRAYSIZE(kIndexData));

        const aoe::GPUBufferDescription const_buffer_desc = {
            aoe::GPUBufferType::kConstantBuffer,
            aoe::GPUResourceUsage::kDynamic
        };
        constant_buffer_ = device_interface->CreateBuffer<Constants>(const_buffer_desc);

        aoe::GPUTexture2DDescription depth_stencil_desc;
        depth_stencil_desc.width = window_.GetWidth();
        depth_stencil_desc.height = window_.GetHeight();
        depth_stencil_desc.pixel_format = aoe::GPUPixelFormat::kD24_Unorm_S8_Uint;
        depth_stencil_desc.texture_flags = aoe::GPUTextureFlags::kDepthStencil;
        depth_stencil_buffer_ = device_interface->CreateTexture2D(depth_stencil_desc);

        const aoe::GPUTexture2DDescription texture_desc = {
            TEXTURE_WIDTH,
            TEXTURE_HEIGHT,
            aoe::GPUPixelFormat::kR8G8B8A8_Unorm,
            aoe::GPUTextureFlags::kShaderResource,
        };
        texture_ = device_interface->CreateTexture2D<uint32_t>(texture_desc, kTextureData);
    };

    void Terminate() override {
        delete swap_chain_;
        delete depth_state_;
        delete rasterized_state_;
        delete vertex_buffer_;
        delete index_buffer_;
        delete constant_buffer_;
        delete depth_stencil_buffer_;
        delete sampler_;
        delete texture_;
    };

    void PerTickUpdate(float dt) override {
        float background_color[4] = { 1.01f, 0.01f, 0.01f, 1.0f };
        
        auto context = device_.GetContext();
        context.ClearState();

        context.ClearRenderTarget(swap_chain_->GetRenderTargetView(), background_color);
        context.ClearDepth(depth_stencil_buffer_->GetTextureView(), 1.0f);

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

        context.SetRenderTarget(swap_chain_->GetRenderTargetView(), depth_stencil_buffer_->GetTextureView());
        context.SetDepthState(depth_state_);

        context.DrawIndexed(index_buffer_->GetElementsCount());

        swap_chain_->Present();
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

    aoe::DX11GPUDevice device_;
    aoe::IGPUSwapChain* swap_chain_;
    aoe::IGPUTexture2D* depth_stencil_buffer_;
    aoe::IGPUDepthState* depth_state_;
    aoe::IGPURasterizerState* rasterized_state_;

    aoe::IGPUBuffer* vertex_buffer_;
    aoe::IGPUBuffer* index_buffer_;
    aoe::IGPUBuffer* constant_buffer_;

    aoe::DX11GPUVertexShader vertex_shader_;
    aoe::DX11GPUPixelShader pixel_shader_;

    aoe::IGPUSampler* sampler_;

    aoe::IGPUTexture2D* texture_;

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