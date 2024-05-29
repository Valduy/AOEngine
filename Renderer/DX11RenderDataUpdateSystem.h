#pragma once

#include "../Game/IECSSystem.h"
#include "../Game/ServiceProvider.h"
#include "../Game/TransformComponent.h"
#include "../Game/Relationeer.h"
#include "../ECS/World.h"

namespace aoe {

class DX11RenderDataUpdateSystem : public IECSSystem {
public:
	DX11RenderDataUpdateSystem(const aoe::ServiceProvider& service_provider);

	void Initialize() override;
	void Terminate() override;

	void PerTickUpdate(float dt) override;
	void PerFrameUpdate(float dt) override;

private:
	const ServiceProvider& service_provider_;

	World* world_;
	Relationeer<TransformComponent>* relationeer_;

	void InitializeGeometryData();
	void InitializeAmbientLightData();
	void InitializeDirectionLightData();

	void SubscribeToComponents();
	void UnsibscribeFromComponents();

	void OnTransformComponentAdded(Entity entity);
	void OnRenderComponentAdded(Entity entity);
	void OnAmbientLightComponentAdded(Entity entity);
	void OnDirectionLightComponentAdded(Entity entity);
	void OnTransformComponentRemoved(Entity entity);
	void OnRenderComponentRemoved(Entity entity);
	void OnAmbientLightComponentRemoved(Entity entity);
	void OnDirectionLightComponentRemoved(Entity entity);

	void UpdateGeometryData(Entity camera);
	void UpdateAmbientLightData();
	void UpdateDirectionLightData(Entity camera);
};

} // namespace aoe