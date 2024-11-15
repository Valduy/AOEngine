#pragma once

#include "../Graphics/DX11GPUBlendState.h"

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11PreLightPassTickSystem : public DX11RenderPassSystemBase {
public:
	DX11PreLightPassTickSystem();

	void Update(float dt) override;

private:
	DX11GPUBlendState blend_state_;

	static GPUBlendStateDescription CreateBlendStateDescription();

	void PrepareRenderContext();
};

} // namespace aoe