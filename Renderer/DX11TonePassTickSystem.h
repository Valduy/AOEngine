#pragma once

#include "../Graphics/DX11GPUContext.h"

#include "DX11RenderPassSystemBase.h"
#include "DX11ShaderHelper.h"

namespace aoe {

class DX11TonePassTickSystem : public DX11RenderPassSystemBase {
public:
	DX11TonePassTickSystem();

	void Update(float dt) override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;

	static GPUSamplerDescription CreateSamplerDescription();
	static GPUBlendStateDescription CreateBlendStateDescription();

	void Render();
	void PrepareRenderContext();
};

} // namespace aoe