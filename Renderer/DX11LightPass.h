#pragma once

#include "DX11AmbientLightPass.h"
#include "DX11DirectionalLightPass.h"

namespace aoe {

class DX11LightPass : public IRenderPass {
public:
	DX11LightPass(const ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	const ServiceProvider& service_provider_;

	DX11GPURenderTargets render_targets_;
	DX11GPUBlendState blend_state_;
	DX11AmbientLightPass ambient_light_pass_;
	DX11DirectionalLightPass directional_light_pass_;

	World* world_;
	DX11RenderContext* render_context_;

	static GPUBlendStateDescription CreateBlendStateDescription();

	void InitializeRenderTargets();
	void PrepareRenderContext();
};

} // namespace aoe