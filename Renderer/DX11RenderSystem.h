#pragma once

#include "../Game/ECSSystemBase.h"

#include "DX11GeometryPass.h"
#include "DX11LightPass.h"
#include "DX11TonePass.h"
#include "DX11DebugPass.h"

namespace aoe {

class DX11RenderSystem : public ECSSystemBase {
public:
	DX11RenderSystem();

	void Initialize(const aoe::ServiceProvider& service_provider) override;
	void Terminate() override;

	void PerTickUpdate(float dt) override;
	void PerFrameUpdate(float dt) override;

private:
	DX11GeometryPass geometry_pass_;
	DX11LightPass light_pass_;
	DX11TonePass tone_pass_;
	DX11DebugPass debug_pass_;

	DX11RenderContext* render_context_;

	void PrepareRenderContext();
	void UpdateCameras();
	void Render();
	void PresentFrame();
	void ClearContext();
};

} // namespace aoe