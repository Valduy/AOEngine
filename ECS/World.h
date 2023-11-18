#pragma once

#include <vector>
#include <unordered_map>

#include "../Core/Debug.h"
#include "../Core/Identifier.h"

#include "Pool.h"
#include "Entity.h"

namespace aoe {

class World {
public:
	bool IsValid(Entity entity) {
		AOE_ASSERT_MSG(entity.GetId() >= 0, "Invalid entity.");

		if (entity.GetId() >= sparse_.size()) {
			return false;
		}

		Lookup lookup = sparse_[entity.GetId()];

		if (lookup >= bound_) {
			return false;
		}

		return dense_[lookup].GetVersion() == entity.GetVersion();
	}

	Entity Create() {
		if (bound_ < dense_.size()) {
			Entity entity = dense_[bound_];
			bound_ += 1;
			return entity;
		}

		AOE_ASSERT_MSG(bound_ == dense_.size(), "Invalid bound.");
		sparse_.push_back(bound_);
		bound_ += 1;
		return dense_.emplace_back(dense_.size());
	}

	void Destroy(Entity entity) {
		if (IsValid(entity)) {
			to_destroy_.push_back(entity);
		}
	}

	template<typename TComponent>
	bool Has(Entity entity) {
		AssertEntityIsValid(entity);
		Pool<TComponent>* pool = GetPool<TComponent>();

		if (pool == nullptr) {
			return false;
		}

		return pool->Has(entity.GetId());
	}

	template<typename TComponent, typename ...TArgs>
	void Add(Entity entity, TArgs&&... args) {
		AssertEntityIsValid(entity);
		Pool<TComponent>* pool = GetPool<TComponent>();

		if (pool == nullptr) {
			pool = CreatePool<TComponent>();
		}

		pool->Add(entity.GetId(), std::forward<TArgs>(args)...);
	}

	template<typename TComponent>
	ComponentHandler<TComponent> Get(Entity entity) {
		AssertEntityIsValid(entity);
		Pool<TComponent>* pool = GetPool<TComponent>();
		return { pool, entity.GetId() };
	}

	template<typename TComponent>
	void Remove(Entity entity) {
		AssertEntityIsValid(entity);
		Pool<TComponent>* pool = GetPool<TComponent>();

		if (pool != nullptr) {
			pool->Remove(entity.GetId());
		}
	}

	void Validate() {
		for (Entity entity : to_destroy_) {
			if (!IsValid(entity)) {
				continue;
			}

			bound_ -= 1;
			Entity moved = dense_[bound_];

			std::swap(sparse_[entity.GetId()], sparse_[moved.GetId()]);
			dense_[sparse_[moved.GetId()]] = moved;
			dense_[sparse_[entity.GetId()]] = {entity.GetVersion() + 1, entity.GetId()};
			
			for (auto& it : pools_) {
				it.second->Remove(entity.GetId());
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
			Entity entity = dense_[i];

			if ((!std::get<Pool<TComponents>*>(pools)->Has(entity.GetId()) || ...)) {
				continue;
			}

			function(entity, ComponentHandler<TComponents>(std::get<Pool<TComponents>*>(pools), entity.GetId())...);
		}
	}

private:
	using Lookup = size_t;

	std::unordered_map<TypeId, IPool*> pools_;
	std::vector<Lookup> sparse_;
	std::vector<Entity> dense_;
	std::vector<Entity> to_destroy_;

	Lookup bound_ = 0;

	void AssertEntityIsValid(Entity entity) {
		AOE_ASSERT_MSG(IsValid(entity), "Invalid entity.");
	}

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