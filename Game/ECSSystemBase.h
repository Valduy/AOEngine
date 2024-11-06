#pragma once

#include "../ECS/World.h"

#include "ServiceProvider.h"

namespace aoe {

class ECSSystemBase {
public:
	ECSSystemBase();
	virtual ~ECSSystemBase() = default;

	World* GetWorld();

	virtual void Initialize(const aoe::ServiceProvider& service_provider);
	virtual void Terminate() {}
	virtual void PerTickUpdate(float dt) {}
	virtual void PerFrameUpdate(float dt) {}

private:
	World* world_;
};

} // namespace aoe