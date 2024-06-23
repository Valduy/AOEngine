#pragma once

#include "../Graphics/DX11GPUContext.h"

#include "DX11RenderPassBase.h"
#include "DX11ShaderHelper.h"

namespace aoe {

class DX11TonePass : public DX11RenderPassBase {
public:
	DX11TonePass();

	void Update() override {}
	void Render() override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;

	static GPUSamplerDescription CreateSamplerDescription();
	static GPUBlendStateDescription CreateBlendStateDescription();

	void PrepareRenderContext();
};

} // namespace aoe