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

struct ModelData {
    aoe::Matrix4 world_view_projection;
    aoe::Matrix4 inverse_transpose_world;
};

struct LightData {
    aoe::Vector3 view_position;
    float dummy0;
    aoe::Vector3 direction;
    float dummy1;
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
        , model_(aoe::ModelLoader::Load(L"Content/Dice_d4.fbx", aoe::ModelLoaderOptions::kFlipUVs))
        , image_(aoe::TextureLoader::Load(L"Content/Dice_d4_Albedo.png", 4))
        , swap_chain_(window)
        , depth_stencil_buffer_(aoe::GPUTexture2DDescription::DepthStencilBuffer(window_.GetWidth(), window_.GetHeight()))
        , depth_state_(kDepthStateDesc)
        , rasterized_state_(kRasterizerStateDesc)
        , vertex_buffer_(CreateVertexBuffer(model_.GetMeshes().back()))
        , index_buffer_(CreateIndexBuffer(model_.GetMeshes().back()))
        , model_buffer_(aoe::DX11GPUBuffer::Create<ModelData>(kConstBufferDesc))
        , light_buffer_(aoe::DX11GPUBuffer::Create<LightData>(kConstBufferDesc))
        , vertex_shader_(aoe::DX11GPUShadersCompiler::CompileShader({aoe::GPUShaderType::kVertex, L"Content/shaders.hlsl", "VertexMain"}))
        , pixel_shader_(aoe::DX11GPUShadersCompiler::CompileShader({aoe::GPUShaderType::kPixel, L"Content/shaders.hlsl", "PixelMain"}))
        , sampler_(kSamplerDesc)
        , texture_(CreateTexture(image_))
    {}

    void Initialize() override {};

    void Terminate() override {};

    void PerTickUpdate(float dt) override {
        float background_color[4] = { 0.15f, 0.15f, 0.15f, 1.0f };
        
        aoe::DX11GPUContext context = aoe::DX11GPUDevice::Instance().GetContext();
        context.ClearState();

        context.ClearRenderTarget(swap_chain_.GetRenderTargetView(), background_color);
        context.ClearDepth(depth_stencil_buffer_.GetTextureView(), 1.0f);

        context.SetRenderTarget(swap_chain_.GetRenderTargetView(), depth_stencil_buffer_.GetTextureView());
        context.SetDepthState(depth_state_);

        context.SetVertexBuffer(vertex_buffer_);
        context.SetIndexBuffer(index_buffer_);

        context.SetVertexShader(vertex_shader_);
        context.SetPixelShader(pixel_shader_);

        context.SetConstantBuffer(aoe::GPUShaderType::kVertex, model_buffer_, 0);
        context.SetConstantBuffer(aoe::GPUShaderType::kPixel, light_buffer_, 1);

        context.SetViewport(viewport_);
        context.SetRasterizerState(rasterized_state_);
        
        context.SetShaderResource(aoe::GPUShaderType::kPixel, texture_.GetTextureView(), 0);
        context.SetSampler(aoe::GPUShaderType::kPixel, sampler_);

        context.SetPrimitiveTopology(aoe::GPUPrimitiveTopology::kTriangleList);
        context.DrawIndexed(index_buffer_.GetElementsCount());

        swap_chain_.Present();
    };

    void PerFrameUpdate(float dt) override {
        viewport_ = GetViewport();

        const float aspect = viewport_.width / viewport_.height;
        auto context = aoe::DX11GPUDevice::Instance().GetContext();

        aoe::Matrix4 world = aoe::Matrix4::Identity();
        world *= aoe::Matrix4::FromTranslationVector(translation_);
        world *= aoe::Quaternion::FromEulerAngles(rotation_).ToMatrix4();
        world *= aoe::Matrix4::FromScaleVector(scale_);
        
        rotation_.x += 0.5f * dt;
        rotation_.y += 0.9f * dt;
        rotation_.z += 0.1f * dt;

        const float fov = aoe::Math::kPi / 2;
        const float near_plain = 0.1f;
        const float far_plain = 100.0f;
        const float handedness = -1.0f;

        aoe::Matrix4 projection_matrix = aoe::Matrix4::Perspective(
            fov, aspect, near_plain, far_plain, handedness);

        aoe::Matrix4 world_view_projection = projection_matrix * world;

        ModelData model_data{};
        model_data.world_view_projection = world_view_projection.Transpose();
        model_data.inverse_transpose_world = world.Inverse();
        context.UpdateBuffer<ModelData>(model_buffer_, &model_data, 1);

        LightData light_data{};
        light_data.view_position = aoe::Math::kV3Zero;
        light_data.direction = aoe::Math::kV3AxisZ;
        context.UpdateBuffer<LightData>(light_buffer_, &light_data, 1);
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
    aoe::DX11GPUBuffer model_buffer_;
    aoe::DX11GPUBuffer light_buffer_;

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