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
	virtual void Update(float dt) = 0;

protected:
	EventBase<Entity>& EntityCreated();
	EventBase<Entity>& EntityDestroyed();

	template<typename TComponent>
	EventBase<Entity>& ComponentAdded();

	template<typename TComponent>
	EventBase<Entity>& ComponentRemoved();

	bool IsEntityValid(Entity entity) const;
	Entity CreateEntity();
	void DestroyEntity(Entity entity);

	template<typename TComponent>
	bool HasComponent(Entity entity) const;

	template<typename TComponent, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&&... args);

	template<typename TComponent>
	CH<TComponent> GetComponent(Entity entity);

	template<typename TComponent>
	void RemoveComponent(Entity entity);

	template <typename ...TComponents>
	auto FilterEntities();

private:
	World* world_;
};

template<typename TComponent>
EventBase<Entity>& ECSSystemBase::ComponentAdded() {
	return world_->ComponentAdded<TComponent>();
}

template<typename TComponent>
EventBase<Entity>& ECSSystemBase::ComponentRemoved() {
	return world_->ComponentRemoved<TComponent>();
}

template<typename TComponent>
bool ECSSystemBase::HasComponent(Entity entity) const {
	return world_->HasComponent<TComponent>(entity);
}

template<typename TComponent, typename ...TArgs>
void ECSSystemBase::AddComponent(Entity entity, TArgs&&... args) {
	world_->AddComponent<TComponent>(entity, std::forward<TArgs>(args)...);
}

template<typename TComponent>
CH<TComponent> ECSSystemBase::GetComponent(Entity entity) {
	return world_->GetComponent<TComponent>(entity);
}

template<typename TComponent>
void ECSSystemBase::RemoveComponent(Entity entity) {
	world_->RemoveComponent<TComponent>(entity);
}

template <typename ...TComponents>
auto ECSSystemBase::FilterEntities() {
	return world_->FilterEntities<TComponents...>();
}

} // namespace aoe