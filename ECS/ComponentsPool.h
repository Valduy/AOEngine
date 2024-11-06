#pragma once

#include "../Core/Event.h"

#include "IComponentsPool.h"
#include "Entity.h"
#include "SparseMap.h"

namespace aoe {

template<typename TComponent>
class ComponentsPool : public IComponentsPool {
public:
	Event<ComponentsPool, Entity> ComponentAdded;
	Event<ComponentsPool, Entity> ComponentRemoved;

	ComponentsPool()
		: sparse_map_()
	{}

	bool Has(Entity entity) override {
		return sparse_map_.Has(entity.GetId());
	}

	TComponent* Get(Entity entity) {
		if (sparse_map_.Has(entity.GetId())) {
			TComponent& component = sparse_map_.Get(entity.GetId());
			return &component;
		}

		return nullptr;
	}

	template<typename ...TArgs>
	void Emplace(Entity entity, TArgs&&... args) {
		TComponent component(std::forward<TArgs>(args)...);
		Add(entity, std::move(component));
	}

	void Add(Entity entity, const TComponent& component) {
		TComponent copy(component);
		Add(entity, std::move(copy));
	}

	void Add(Entity entity, TComponent&& component) {
		if (sparse_map_.Has(entity.GetId())) {
			Remove(entity);
		}

		sparse_map_.Add(entity.GetId(), std::forward<TComponent>(component));
		ComponentAdded.Notify(entity);
	}

	void Remove(Entity entity) override {
		if (!sparse_map_.Has(entity.GetId())) {
			return;
		}

		ComponentRemoved.Notify(entity);
		sparse_map_.Remove(entity.GetId());
	}

private:
	SparseMap<TComponent> sparse_map_;
};

} // namespace aoe