#pragma once

#include "../Resources/DX11ModelManager.h"

#include "DX11RenderPassBase.h"

namespace aoe {

class DX11PointLightPass : public DX11RenderPassBase {
public:
	DX11PointLightPass();

	void Initialize(const aoe::ServiceProvider& service_provider) override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;

	DX11ModelManager* model_manager_;
	ModelId sphere_id;

	void InitializePointLightData();

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void PrepareRenderContext();
	
	void OnPointLightComponentAdded(Entity entity);
	void OnTransformComponentAdded(Entity entity);

	void OnComponentRemoved(Entity entity);

	void SetupPointLight(Entity entity);
};

} // namespace aoe