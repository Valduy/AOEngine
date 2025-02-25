#pragma once

#include "../Graphics/DX11GPUVertexShader.h"
#include "../Graphics/DX11GPUPixelShader.h"
#include "../Graphics/DX11GPUSampler.h"
#include "../Graphics/DX11GPUBlendState.h"
#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11GeometryPassSystem : public DX11RenderPassSystemBase {
public:
	DX11GeometryPassSystem();

	void Initialize(const ServiceProvider& service_provider) override;
	void Update(float dt) override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;
	
	DX11ModelManager* model_manager_;
	DX11TextureManager* texture_manager_;

	static GPUSamplerDescription CreateSamplerDescription();
	static GPUBlendStateDescription CreateBlendStateDescription();

	void UpdateRenderData();
	void Render(Entity camera);
	void PrepareRenderContext();
};

} // namespace aoe