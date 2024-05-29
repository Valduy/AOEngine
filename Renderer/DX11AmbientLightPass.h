#pragma once

#include "../Game/ServiceProvider.h"
#include "../Graphics/DX11GPUContext.h"
#include "../ECS/World.h"

#include "IRenderPass.h"
#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"
#include "DX11RenderDataComponents.h"

namespace aoe {

class DX11AmbientLightPass : public IRenderPass {
public:
	DX11AmbientLightPass(const aoe::ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void Render() override;

private:
	const ServiceProvider& service_provider_;

	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;

	World* world_;
	DX11RenderContext* render_context_;

	void PrepareRenderContext();
};

} // namespace aoe