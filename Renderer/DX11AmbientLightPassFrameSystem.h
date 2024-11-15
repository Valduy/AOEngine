#pragma once

#include "DX11RenderPassSystemBase.h"

namespace aoe {

class DX11AmbientLightPassFrameSystem : public DX11RenderPassSystemBase {
public:
	void Initialize(const ServiceProvider& service_provider) override;
	void Terminate() override;
	void Update(float dt) override;

private:
	void InitializeAmbientLightData();
	void UpdateRenderData();

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void OnAmbientLightComponentAdded(Entity entity);
	void OnAmbientLightComponentRemoved(Entity entity);
};

} // namespace aoe