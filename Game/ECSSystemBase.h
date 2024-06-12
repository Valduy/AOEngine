#pragma once

#include "../ECS/World.h"

#include "ServiceProvider.h"

namespace aoe {

class ECSSystemBase {
public:
	ECSSystemBase()
		: world_(nullptr)
	{}

	virtual ~ECSSystemBase() = default;

	World* GetWorld() {
		return world_;
	}

	virtual void Initialize(const aoe::ServiceProvider& service_provider) {
		world_ = service_provider.GetService<World>();
		AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");
	}

	virtual void Terminate() {}
	virtual void PerTickUpdate(float dt) {}
	virtual void PerFrameUpdate(float dt) {}

private:
	World* world_;
};

} // namespace aoe