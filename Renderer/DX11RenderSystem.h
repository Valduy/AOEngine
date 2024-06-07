#pragma once

#include "../Game/IECSSystem.h"

#include "DX11GeometryPass.h"
#include "DX11LightPass.h"
#include "DX11TonePass.h"
#include "DX11DebugPass.h"

namespace aoe {

class DX11RenderSystem : public IECSSystem {
public:
	DX11RenderSystem(const aoe::ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void PerTickUpdate(float dt) override;
	void PerFrameUpdate(float dt) override;

private:
	const ServiceProvider& service_provider_;
	
	DX11GeometryPass geometry_pass_;
	DX11LightPass light_pass_;
	DX11TonePass tone_pass_;
	DX11DebugPass debug_pass_;

	World* world_;
	DX11RenderContext* render_context_;

	void PrepareRenderContext();
	void Render();
	void PresentFrame();
};

} // namespace aoe