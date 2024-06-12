#pragma once

#include <vector>

#include "../Core/Event.h"
#include "../Core/Debug.h"

#include "IPool.h"
#include "Entity.h"
#include "ComponentHandler.h"

namespace aoe {

template<typename TComponent>
class Pool : public IPool {
public:
	friend class ComponentHandler<TComponent>;

	Event<Pool, Entity> ComponentAdded;
	Event<Pool, Entity> ComponentRemoved;

	Pool()
		: sparse_()
		, dense_()
		, bound_(0)
	{}

	bool Has(Entity entity) override {
		AssertEntityIsValid(entity);

		if (entity.GetId() >= sparse_.size()) {
			return false;
		}

		Lookup lookup = sparse_[entity.GetId()];
		return lookup != kInvalid;
	}

	TComponent* Get(Entity entity) {
		AssertEntityIsValid(entity);

		if (entity.GetId() >= sparse_.size()) {
			return nullptr;
		}

		Lookup lookup = sparse_[entity.GetId()];

		if (lookup == kInvalid) {
			return nullptr;
		}

		ComponentHolder& holder = dense_[lookup];
		return &holder.data;
	}

	template<typename ...TArgs>
	void Add(Entity entity, TArgs&&... args) {
		AssertEntityIsValid(entity);

		if (entity.GetId() >= sparse_.size()) {
			sparse_.resize(entity.GetId() + 1, kInvalid);
		}

		Lookup lookup = sparse_[entity.GetId()];

		if (lookup == kInvalid) {
			sparse_[entity.GetId()] = CreateComponent(entity, std::forward<TArgs>(args)...);
		}
		else {
			ComponentRemoved.Notify(entity);
			ComponentHolder& holder = dense_[lookup];
			TComponent component(std::forward<TArgs>(args)...);
			holder.data = std::move(component);
		}

		ComponentAdded.Notify(entity);
	}

	void Remove(Entity entity) override {
		AssertEntityIsValid(entity);

		if (entity.GetId() >= sparse_.size() || sparse_[entity.GetId()] == kInvalid) {
			return;
		}
				
		ComponentRemoved.Notify(entity);
		RemoveComponent(entity);
	}

private:
	using Lookup = std::int32_t;

	struct ComponentHolder {
		EntityId owner;
		TComponent data;

		ComponentHolder(EntityId entity_id, TComponent component)
			: owner(entity_id)
			, data(std::move(component))
		{}
	};

	static const Lookup kInvalid = -1;

	std::vector<Lookup> sparse_;
	std::vector<ComponentHolder> dense_;

	// IF: lookup < bound THEN: dense_[lookup] is a living component
	// ELSE: dense_[lookup] is a dead component
	Lookup bound_;

	static void AssertEntityIsValid(Entity entity) {
		AOE_ASSERT_MSG(entity.GetId() >= 0 && entity.GetVersion() >= 0, "Invalid entity.");
	}

	template<typename ...TArgs>
	Lookup CreateComponent(Entity entity, TArgs&&... args) {
		AOE_ASSERT_MSG(bound_ <= dense_.size(), "Invalid bound.");
		
		TComponent component(std::forward<TArgs>(args)...);
		Lookup lookup = bound_;

		if (lookup < dense_.size()) {
			ComponentHolder& holder = dense_[lookup];
			holder.owner = entity.GetId();
			holder.data = std::move(component);
		}
		else {
			dense_.emplace_back(entity.GetId(), std::move(component));
		}

		bound_ += 1;
		return lookup;
	}

	void RemoveComponent(Entity entity) {
		bound_ -= 1;
		Lookup lookup = sparse_[entity.GetId()];
		EntityId moved = dense_[bound_].owner;
		std::swap(dense_[lookup], dense_[bound_]);
		sparse_[moved] = lookup;
		sparse_[entity.GetId()] = kInvalid;
	}

};

} // namespace aoe