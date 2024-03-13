#include "../Core/Math.h"
#include "../Application/Application.h"
#include "../Resources/ModelLoader.h"
#include "../Resources/TextureLoader.h"
#include "../Graphics/DX11GPUDevice.h"
#include "../Graphics/DX11GPUContext.h"
#include "../Graphics/DX11GPUSwapChain.h"
#include "../Graphics/DX11GPUDepthState.h"
#include "../Graphics/DX11GPURasterizerState.h"
#include "../Graphics/DX11GPUBuffer.h"
#include "../Graphics/DX11GPUShadersCompiler.h"
#include "../Graphics/DX11GPUSampler.h"

struct Constants {
    aoe::Matrix4 transform;
    aoe::Matrix4 projection;
    aoe::Vector3 light_vector;
    float dummy;
};

static const aoe::GPUDepthStateDescription kDepthStateDesc = {
    true,
    aoe::GPUDepthWriteMask::kWriteAll,
    aoe::GPUComparsionFunction::kLess
};

static const aoe::GPURasterizerStateDescription kRasterizerStateDesc = {
    aoe::GPUCullMode::kBack,
    aoe::GPUFillMode::kSolid,
};

static const aoe::GPUBufferDescription kVertexBufferDesc = {
    aoe::GPUBufferType::kVertexBuffer,
    aoe::GPUResourceUsage::kDefault
};

static const aoe::GPUBufferDescription kIndexBufferDesc = {
    aoe::GPUBufferType::kIndexBuffer,
    aoe::GPUResourceUsage::kDefault
};

static const aoe::GPUBufferDescription kConstBufferDesc = {
    aoe::GPUBufferType::kConstantBuffer,
    aoe::GPUResourceUsage::kDynamic
};

static const aoe::GPUSamplerDescription kSamplerDesc = {};

class DX11MinimalScene : public aoe::IScene {
public:
    DX11MinimalScene(const aoe::Window& window)
        : window_(window)
        , model_(aoe::ModelLoader::Load("Content/Dice_d4.fbx"))
        , image_(aoe::TextureLoader::Load("Content/Dice_d4_Albedo.png", 4))
        , swap_chain_(window)
        , depth_stencil_buffer_(aoe::GPUTexture2DDescription::DepthStencilBuffer(window_.GetWidth(), window_.GetHeight()))
        , depth_state_(kDepthStateDesc)
        , rasterized_state_(kRasterizerStateDesc)
        , vertex_buffer_(CreateVertexBuffer(model_.GetMeshes().back()))
        , index_buffer_(CreateIndexBuffer(model_.GetMeshes().back()))
        , constant_buffer_(aoe::DX11GPUBuffer::Create<Constants>(kConstBufferDesc))
        , vertex_shader_(aoe::DX11GPUShadersCompiler::CompileShader({aoe::GPUShaderType::kVertex, L"Content/shaders.hlsl", "vs_main"}))
        , pixel_shader_(aoe::DX11GPUShadersCompiler::CompileShader({aoe::GPUShaderType::kPixel, L"Content/shaders.hlsl", "ps_main"}))
        , sampler_(kSamplerDesc)
        , texture_(CreateTexture(image_))
    {}

    void Initialize() override {};

    void Terminate() override {};

    void PerTickUpdate(float dt) override {
        float background_color[4] = { 0.15f, 0.15f, 0.15f, 1.0f };
        
        auto context = aoe::DX11GPUDevice::Instance()->GetContext();
        context.ClearState();

        context.ClearRenderTarget(swap_chain_.GetRenderTargetView(), background_color);
        context.ClearDepth(depth_stencil_buffer_.GetTextureView(), 1.0f);

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

        context.SetRenderTarget(swap_chain_.GetRenderTargetView(), depth_stencil_buffer_.GetTextureView());
        context.SetDepthState(depth_state_);

        context.DrawIndexed(index_buffer_.GetElementsCount());

        swap_chain_.Present();
    };

    void PerFrameUpdate(float dt) override {
        viewport_ = GetViewport();

        const float aspect = viewport_.width / viewport_.height;
        auto context = aoe::DX11GPUDevice::Instance()->GetContext();

        aoe::Matrix4 model = aoe::Matrix4::Identity();
        model *= aoe::Matrix4::FromTranslationVector(translation_);
        model *= aoe::Quaternion::FromEulerAngles(rotation_).ToMatrix4();
        model *= aoe::Matrix4::FromScaleVector(scale_);
        
        rotation_.x += 0.5f * dt;
        rotation_.y += 0.9f * dt;
        rotation_.z += 0.1f * dt;

        const float fov = aoe::Math::kPi / 3;
        const float near_plain = 0.1f;
        const float far_plain = 100.0f;
        const float handedness = -1.0f;

        aoe::Matrix4 projection = aoe::Matrix4::Perspective(
            fov, aspect, near_plain, far_plain, handedness);

        Constants constants{};
        constants.transform = model.Transpose();
        constants.projection = projection.Transpose();
        constants.light_vector = { 1.0f, -1.0f, 1.0f };
        context.UpdateBuffer<Constants>(constant_buffer_, &constants, 1);
    };

private:
    const aoe::Window& window_;

    aoe::Model model_;
    aoe::Image image_;

    aoe::DX11GPUSwapChain swap_chain_;
    aoe::DX11GPUTexture2D depth_stencil_buffer_;
    aoe::DX11GPUDepthState depth_state_;
    aoe::DX11GPURasterizerState rasterized_state_;

    aoe::DX11GPUBuffer vertex_buffer_;
    aoe::DX11GPUBuffer index_buffer_;
    aoe::DX11GPUBuffer constant_buffer_;

    aoe::DX11GPUVertexShader vertex_shader_;
    aoe::DX11GPUPixelShader pixel_shader_;

    aoe::DX11GPUSampler sampler_;

    aoe::DX11GPUTexture2D texture_;

    aoe::Viewport viewport_;

    aoe::Vector3 rotation_ = { 0.0f, 0.0f, 0.0f };
    aoe::Vector3 scale_ = { 1.0f, 1.0f, 1.0f };
    aoe::Vector3 translation_ = { 0.0f, 0.0f, 4.0f };

    static aoe::DX11GPUBuffer CreateVertexBuffer(const aoe::Mesh& mesh) {
        const std::vector<aoe::Vertex>& vertices = mesh.GetVertices();
        uint32_t size = static_cast<uint32_t>(vertices.size());
        return aoe::DX11GPUBuffer::Create<aoe::Vertex>(kVertexBufferDesc, vertices.data(), size);
    }

    static aoe::DX11GPUBuffer CreateIndexBuffer(const aoe::Mesh& mesh) {
        const std::vector<aoe::Index>& indices = mesh.GetIndices();
        uint32_t size = static_cast<uint32_t>(indices.size());
        return aoe::DX11GPUBuffer::Create<aoe::Index>(kIndexBufferDesc, indices.data(), size);
    }

    static aoe::DX11GPUTexture2D CreateTexture(const aoe::Image& image) {
        aoe::GPUTexture2DDescription texture_desc{
            image.GetWidth(),
            image.GetHeight(),
            aoe::GPUPixelFormat::kR8G8B8A8_Unorm,
            aoe::GPUTextureFlags::kShaderResource,
        };

        return { texture_desc, image.GetData(), image.GetChannels() };
    }

    aoe::Viewport GetViewport() {
        const float width = static_cast<float>(window_.GetWidth());
        const float height = static_cast<float>(window_.GetHeight());

        return { 0.0f, 0.0f, width, height, 0.0f, 1.0f };
    }
};

int main() {
    aoe::Application application(L"Game", 800, 600);
    DX11MinimalScene scene(application.GetWindow());
    application.Start(scene);

    return 0;
}