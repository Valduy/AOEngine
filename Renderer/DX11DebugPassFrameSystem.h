#pragma once

#include "../Graphics/DX11GPUBuffer.h"

#include "DX11RenderPassSystemBase.h"
#include "Line.h"

namespace aoe {

class DX11DebugPassFrameSystem : public DX11RenderPassSystemBase {
public:
	void Initialize(const ServiceProvider& service_provider) override;
	void Terminate();
	void Update(float dt) override;

private:
	void InitializeLineData();
	void UpdateRenderData(Entity camera);

	std::vector<DX11GPUBuffer> CreateLineResources(const Line& lines);
	DX11GPUBuffer CreateVertexBuffer(const Segment& segment);

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void OnTransformComponentAdded(Entity entity);
	void OnLineComponentAdded(Entity entity);
	void OnComponentRemoved(Entity entity);

	void SetupLineEntity(Entity entity);
};

} // namespace aoe