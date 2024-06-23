#pragma once

#include "DX11AmbientLightPass.h"
#include "DX11DirectionalLightPass.h"

namespace aoe {

class DX11LightPass : public DX11RenderPassBase {
public:
	DX11LightPass();

	void Initialize(const aoe::ServiceProvider& service_provider) override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	DX11GPUBlendState blend_state_;
	DX11AmbientLightPass ambient_light_pass_;
	DX11DirectionalLightPass directional_light_pass_;

	static GPUBlendStateDescription CreateBlendStateDescription();

	void PrepareRenderContext();
};

} // namespace aoe