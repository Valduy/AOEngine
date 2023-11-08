#pragma once

#include <cassert>
#include <vector>
#include <unordered_map>

#include "../Core/Identifier.h"

#include "Pool.h"
#include "Entity.h"

namespace aoe {

class World {
public:
	Version GetVersion(EntityId entity_id) {
		assert(entity_id >= 0 && "Invalid entity.");

		if (entity_id < sparse_.size()) {
			return sparse_[entity_id].version;
		}

		return kNullVersion;
	}

	bool IsValid(EntityId entity_id) {
		assert(entity_id >= 0 && "Invalid entity.");
		EntityDescriptor& descriptor = sparse_[entity_id];
		return descriptor.lookup < bound_;
	}

	EntityId Create() {
		if (bound_ < dense_.size()) {
			EntityId entity_id = dense_[bound_];
			bound_ += 1;
			return entity_id;
		}

		assert(bound_ == dense_.size() && "Invalid bound.");
		sparse_.emplace_back(kInitialVersion, bound_);
		dense_.push_back(dense_.size());
		EntityId entity_id = bound_;
		bound_ += 1;
		return entity_id;
	}

	void Destroy(EntityId entity_id) {
		if (IsValid(entity_id)) {
			to_destroy_.push_back(entity_id);
		}
	}

	template<typename TComponent>
	bool Has(EntityId entity_id) {
		assert(IsValid(entity_id) && "Invalid entity.");
		Pool<TComponent>* pool = GetPool<TComponent>();

		if (pool == nullptr) {
			return false;
		}

		return pool->Has(entity_id);
	}

	template<typename TComponent, typename ...TArgs>
	void Add(EntityId entity_id, TArgs&&... args) {
		assert(IsValid(entity_id) && "Invalid entity.");
		Pool<TComponent>* pool = GetPool<TComponent>();

		if (pool == nullptr) {
			pool = CreatePool<TComponent>();
		}

		pool->Add(entity_id, std::forward<TArgs>(args)...);
	}

	template<typename TComponent>
	ComponentHandler<TComponent> Get(EntityId entity_id) {
		assert(IsValid(entity_id) && "Invalid entity.");
		Pool<TComponent>* pool = GetPool<TComponent>();
		return { pool, entity_id };
	}

	template<typename TComponent>
	void Remove(EntityId entity_id) {
		assert(IsValid(entity_id) && "Invalid entity.");
		Pool<TComponent>* pool = GetPool<TComponent>();

		if (pool != nullptr) {
			pool->Remove(entity_id);
		}
	}

	void Validate() {
		for (EntityId entity_id : to_destroy_) {
			if (!IsValid(entity_id)) {
				continue;
			}

			bound_ -= 1;
			EntityDescriptor& descriptor = sparse_[entity_id];
			EntityId moved = dense_[bound_];
			std::swap(dense_[descriptor.lookup], dense_[bound_]);
			sparse_[moved].lookup = descriptor.lookup;
			descriptor.lookup = bound_;
			descriptor.version += 1;

			for (auto& it : pools_) {
				it.second->Remove(entity_id);
			}
		}
	}

	template <typename ...TComponents, typename TFunction>
	void ForEach(TFunction function) {
		auto pools(std::make_tuple(GetPool<TComponents>()...));

		if (((std::get<Pool<TComponents>*>(pools) == nullptr) || ...)) {
			return;
		}

		for (size_t i = 0; i < bound_; ++i) {
			EntityId entity_id = dense_[i];

			if ((!std::get<Pool<TComponents>*>(pools)->Has(entity_id) || ...)) {
				continue;
			}

			function(entity_id, ComponentHandler<TComponents>(std::get<Pool<TComponents>*>(pools), entity_id)...);
		}
	}

private:
	using Lookup = EntityId;

	static const Version kInitialVersion = 0;

	struct EntityDescriptor {
		Version version;
		Lookup lookup;

		EntityDescriptor(Version version, Lookup lookup)
			: version(version)
			, lookup(lookup)
		{}
	};

	std::unordered_map<TypeId, IPool*> pools_;
	std::vector<EntityDescriptor> sparse_;
	std::vector<EntityId> dense_;
	std::vector<EntityId> to_destroy_;

	Lookup bound_ = 0;

	template<typename TComponent>
	Pool<TComponent>* GetPool() {
		TypeId type_id = aoe::Identifier::GetTypeId<TComponent>();
		auto it = pools_.find(type_id);

		if (it == pools_.end()) {
			return nullptr;
		}

		return static_cast<Pool<TComponent>*>(it->second);
	}

	template<typename TComponent>
	Pool<TComponent>* CreatePool() {
		TypeId type_id = aoe::Identifier::GetTypeId<TComponent>();
		Pool<TComponent>* pool = new Pool<TComponent>();
		pools_[type_id] = pool;
		return pool;
	}
};

} // namespace aoe