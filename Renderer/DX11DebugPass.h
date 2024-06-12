#pragma once

#include <unordered_map>

#include "../Game/ServiceProvider.h"
#include "../Game/Relationeer.h"
#include "../Game/TransformUtils.h"
#include "../Graphics/DX11GPUBuffer.h"
#include "../ECS/World.h"

#include "IRenderPass.h"
#include "DX11RenderContext.h"
#include "DX11ShaderHelper.h"
#include "LineComponent.h"
#include "CameraUtils.h"

namespace aoe {

class DX11LineResourcesComponent {
public:
	DX11LineResourcesComponent(std::vector<DX11GPUBuffer> line_resources)
		: line_resources_(std::move(line_resources))
	{}

	const std::vector<DX11GPUBuffer>& GetLineResources() {
		return line_resources_;
	}

private:
	std::vector<DX11GPUBuffer> line_resources_;
};

class DX11DebugPass : public IRenderPass {
public:
	DX11DebugPass(const ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void Update() override;
	void Render() override;

private:
	const ServiceProvider& service_provider_;

	DX11GPUVertexShader vertex_shader_;
	DX11GPUPixelShader pixel_shader_;
	
	World* world_;
	DX11RenderContext* render_context_;
	Relationeer<TransformComponent>* relationeer_;

	void InitializeLineData();
	std::vector<DX11GPUBuffer> CreateLineResources(const Line& lines);
	DX11GPUBuffer CreateVertexBuffer(const Segment& segment);

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void UpdateLineData(Entity camera);
	void PrepareRenderContext();

	void OnTransformComponentAdded(Entity entity);
	void OnLineComponentAdded(Entity entity);

	void OnComponentRemoved(Entity entity);

	void SetupLineEntity(Entity entity);
};

} // namespace aoe