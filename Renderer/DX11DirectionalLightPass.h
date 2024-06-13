#pragma once

#include "DX11RenderPassBase.h"

namespace aoe {

class DX11DirectionalLightPass : public DX11RenderPassBase {
public:
	DX11DirectionalLightPass();

	void Initialize(const aoe::ServiceProvider& service_provider) override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;

	void InitializeDirectionLightData();

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void PrepareRenderContext();
	void UpdateDirectionLightData(Entity camera);

	void OnDirectionLightComponentAdded(Entity entity);
	void OnDirectionLightComponentRemoved(Entity entity);
};

}