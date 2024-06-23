#pragma once

#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"

#include "DX11RenderPassBase.h"

namespace aoe {

class DX11GeometryPass : public DX11RenderPassBase {
public:
	DX11GeometryPass();

	void Initialize(const ServiceProvider& service_provider) override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;

	DX11ModelManager* model_manager_;
	DX11TextureManager* texture_manager_;

	static GPUSamplerDescription CreateSamplerDescription();
	static GPUBlendStateDescription CreateBlendStateDescription();

	void InitializeGeometryData();

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void UpdateGeometryData(Entity camera);
	void PrepareRenderContext();

	void OnTransformComponentAdded(Entity entity);
	void OnRenderComponentAdded(Entity entity);

	void OnComponentRemoved(Entity entity);

	void SetupGeometryEntity(Entity entity);
};

} // namespace aoe