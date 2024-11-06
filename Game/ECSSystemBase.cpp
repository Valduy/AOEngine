#include "pch.h"

#include "ECSSystemBase.h"

namespace aoe {

ECSSystemBase::ECSSystemBase()
	: world_(nullptr)
{}

World* ECSSystemBase::GetWorld() {
	return world_;
}

void ECSSystemBase::Initialize(const aoe::ServiceProvider & service_provider) {
	world_ = service_provider.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");
}

} // namespace aoe

