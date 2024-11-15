#include "pch.h"

#include "ECSSystemBase.h"

namespace aoe {

ECSSystemBase::ECSSystemBase()
	: world_(nullptr)
{}

World* ECSSystemBase::GetWorld() {
	return world_;
}

void ECSSystemBase::Initialize(const aoe::ServiceProvider& service_provider) {
	world_ = service_provider.GetService<World>();
	AOE_ASSERT_MSG(world_ != nullptr, "There is no World service.");
}

EventBase<Entity>& ECSSystemBase::EntityCreated() {
	return world_->EntityCreated;
}

EventBase<Entity>& ECSSystemBase::EntityDestroyed() {
	return world_->EntityDestroyed;
}

bool ECSSystemBase::IsValid(Entity entity) const {
	return world_->IsValid(entity);
}

Entity ECSSystemBase::CreateEntity() {
	return world_->Create();
}

void ECSSystemBase::DestroyEntity(Entity entity) {
	world_->Destroy(entity);
}

} // namespace aoe

