#pragma once

#include "DX11RenderPassBase.h"

namespace aoe {

class DX11AmbientLightPass : public DX11RenderPassBase {
public:
	DX11AmbientLightPass();

	void Initialize(const aoe::ServiceProvider& service_provider) override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;

	void InitializeAmbientLightData();

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void PrepareRenderContext();

	void OnAmbientLightComponentAdded(Entity entity);
	void OnAmbientLightComponentRemoved(Entity entity);
};

} // namespace aoe