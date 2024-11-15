#pragma once

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11DirectionalLightPassFrameSystem : public DX11RenderPassSystemBase {
public:
	void Initialize(const ServiceProvider& service_provider) override;
	void Terminate() override;
	void Update(float dt) override;

private:
	void InitializeDirectionLightData();
	void UpdateRenderData(Entity camera);

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void OnDirectionLightComponentAdded(Entity entity);
	void OnDirectionLightComponentRemoved(Entity entity);
};

} // namespace aoe