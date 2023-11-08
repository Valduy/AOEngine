#pragma once

#include <vector>
#include <cassert>

#include "IPool.h"
#include "ComponentHandler.h"

namespace aoe {

template<typename TComponent>
class Pool : public IPool {
	friend class ComponentHandler<TComponent>;

	using Lookup = std::int32_t;

public:
	Pool()
		: sparse_()
		, dense_()
		, bound_(0)
	{}

	bool Has(EntityId entity_id) override {
		assert(entity_id >= 0 && "Invalid entity.");

		if (entity_id >= sparse_.size()) {
			return false;
		}

		Lookup lookup = sparse_[entity_id];
		return lookup != kInvalid;
	}

	TComponent* Get(EntityId entity_id) {
		assert(entity_id >= 0 && "Invalid entity.");

		if (entity_id >= sparse_.size()) {
			return nullptr;
		}

		Lookup lookup = sparse_[entity_id];

		if (lookup == kInvalid) {
			return nullptr;
		}

		ComponentHolder& holder = dense_[lookup];
		return &holder.data;
	}

	template<typename ...TArgs>
	void Add(EntityId entity_id, TArgs&&... args) {
		assert(entity_id >= 0 && "Invalid entity.");

		if (entity_id >= sparse_.size()) {
			sparse_.resize(entity_id + 1, kInvalid);
		}

		Lookup lookup = sparse_[entity_id];

		if (lookup == kInvalid) {
			sparse_[entity_id] = CreateComponent(entity_id, std::forward<TArgs>(args)...);
		}
		else {
			ComponentHolder& holder = dense_[lookup];
			TComponent component(std::forward<TArgs>(args)...);
			holder.data = std::move(component);
		}
	}

	void Remove(EntityId entity_id) override {
		assert(entity_id >= 0 && "Invalid entity.");

		if (entity_id >= sparse_.size() || sparse_[entity_id] == kInvalid) {
			return;
		}

		RemoveComponent(entity_id);
	}

private:
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

	template<typename ...TArgs>
	Lookup CreateComponent(EntityId entity_id, TArgs&&... args) {
		assert(bound_ <= dense_.size() && "Invalid bound.");
		TComponent component(std::forward<TArgs>(args)...);
		Lookup lookup = bound_;

		if (lookup < dense_.size()) {
			ComponentHolder& holder = dense_[lookup];
			holder.owner = entity_id;
			holder.data = std::move(component);
		}
		else {
			dense_.emplace_back(entity_id, std::move(component));
		}

		bound_ += 1;
		return lookup;
	}

	void RemoveComponent(EntityId entity_id) {
		bound_ -= 1;
		Lookup lookup = sparse_[entity_id];
		EntityId moved = dense_[bound_].owner;
		std::swap(dense_[lookup], dense_[bound_]);
		sparse_[moved] = lookup;
		sparse_[entity_id] = kInvalid;
	}

};

} // namespace aoe