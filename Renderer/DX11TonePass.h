#pragma once

#include "../Core/Debug.h"
#include "../Game/ServiceProvider.h"
#include "../Graphics/DX11GPUContext.h"

#include "IRenderPass.h"
#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"

namespace aoe {

class DX11TonePass : public IRenderPass {
public:
	DX11TonePass(const ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void Render() override;

private:
	const ServiceProvider& service_provider_;

	DX11GPURenderTargets render_targets_;
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;

	DX11RenderContext* render_context_;

	static GPUSamplerDescription CreateSamplerDescription();
	static GPUBlendStateDescription CreateBlendStateDescription();

	void InitializeRenderTargets();
	void PrepareRenderContext();
};

} // namespace aoe