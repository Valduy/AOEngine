#pragma once

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11GeometryPassFrameSystem : public DX11RenderPassSystemBase {
public:
	void Initialize(const ServiceProvider& service_provider) override;
	void Terminate() override;
	void Update(float dt) override;

private:
	void InitializeGeometryData();
	void UpdateRenderData(Entity camera);

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void OnTransformComponentAdded(Entity entity);
	void OnRenderComponentAdded(Entity entity);
	void OnComponentRemoved(Entity entity);
};

} // namespace aoe