#pragma once

#include "../Game/ServiceProvider.h"
#include "../Game/Relationeer.h"
#include "../Game/TransformUtils.h"
#include "../Graphics/DX11GPUContext.h"
#include "../Graphics/DX11GPUShadersCompiler.h"
#include "../ECS/World.h"
#include "../Resources/DX11ModelManager.h"
#include "../Resources/DX11TextureManager.h"

#include "IRenderPass.h"
#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"
#include "RenderComponent.h"
#include "DX11RenderDataComponents.h"

namespace aoe {

class DX11GeometryPass : public IRenderPass {
public:
	DX11GeometryPass(const ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	const ServiceProvider& service_provider_;

	DX11GPURenderTargets render_targets_;
	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	DX11GPUSampler sampler_;
	DX11GPUBlendState blend_state_;

	World* world_;
	DX11RenderContext* render_context_;
	DX11ModelManager* model_manager_;
	DX11TextureManager* texture_manager_;
	Relationeer<TransformComponent>* relationeer_;

	static GPUSamplerDescription CreateSamplerDescription();
	static GPUBlendStateDescription CreateBlendStateDescription();

	void InitializeRenderTargets();
	void InitializeGeometryData();

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void UpdateGeometryData(Entity camera);
	void PrepareRenderContext();

	void OnTransformComponentAdded(Entity entity);
	void OnTransformComponentRemoved(Entity entity);

	void OnRenderComponentAdded(Entity entity);
	void OnRenderComponentRemoved(Entity entity);
};

} // namespace aoe