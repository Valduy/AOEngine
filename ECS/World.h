#pragma once

#include <unordered_map>

#include "../Core/Identifier.h"

#include "ComponentsPool.h"
#include "EntitiesPool.h"
#include "ComponentHandler.h"

namespace aoe {

class World {
private:
	template<typename ...TComponents>
	using ComponentsPools = std::tuple<ComponentsPool<TComponents>*...>;
	using InnerIterator = EntitiesPool::Iterator;

public:
	template<typename... TComponents>
	class Filter {
	public:
		class Iterator {
		public:
			using iterator_category = std::forward_iterator_tag;
			using difference_type = std::ptrdiff_t;
			using value_type = Entity;
			using pointer = value_type*;
			using reference = value_type&;

			Iterator(World* world, InnerIterator it, InnerIterator end)
				: pools_(world->GetPools<TComponents...>())
				, it_(it)
				, end_(end)
			{
				if (it_ == end_) {
					return;
				}

				if (World::HasNullPools(pools_)) {
					it_ = end_;
					return;
				}

				if (!World::HasRequiredComponents(pools_, *it_)) {
					Advance();
				}
			}

			reference operator*() {
				return *it_;
			}

			pointer operator->() {
				return &(*it_);
			}

			Iterator& operator++() {
				Advance();
				return *this;
			}

			Iterator operator++(int) {
				Iterator temp = *this;
				Advance();
				return temp;
			}

			friend bool operator== (const Iterator& lhs, const Iterator& rhs) {
				return lhs.it_ == rhs.it_;
			};

			friend bool operator!= (const Iterator& lhs, const Iterator& rhs) {
				return lhs.it_ != rhs.it_;
			};

		private:
			ComponentsPools<TComponents...> pools_;
			InnerIterator it_;
			InnerIterator end_;

			void Advance() {
				for (std::advance(it_, 1); it_ != end_; std::advance(it_, 1)) {
					if (World::HasRequiredComponents(pools_, *it_)) {
						return;
					}
				}
			}
		};

		Filter(World* world)
			: world_(world)
		{}

		Iterator begin() {
			auto it = world_->entities_pool_.begin();
			auto end = world_->entities_pool_.end();
			return { world_, it, end };
		}

		Iterator end() {
			auto it = world_->entities_pool_.end();
			auto end = world_->entities_pool_.end();
			return { world_, it, end };
		}

	private:
		World* world_;
	};

	Event<World, Entity> EntityCreated;
	Event<World, Entity> EntityDestroyed;

	World()
		: component_pools_()
		, entities_pool_()
		, to_destroy_()
	{}

	~World() {
		for (auto it : component_pools_) {
			delete it.second;
		}
	}

	template<typename TComponent>
	EventBase<Entity>& ComponentAdded() {
		ComponentsPool<TComponent>* pool = GetOrCreatePool<TComponent>();
		return pool->ComponentAdded;
	}

	template<typename TComponent>
	EventBase<Entity>& ComponentRemoved() {
		ComponentsPool<TComponent>* pool = GetOrCreatePool<TComponent>();
		return pool->ComponentRemoved;
	}

	bool IsEntityValid(Entity entity) const {
		return entities_pool_.IsValid(entity);
	}

	Entity CreateEntity() {
		return entities_pool_.Create();
	}

	void DestroyEntity(Entity entity) {
		if (IsEntityValid(entity)) {
			to_destroy_.push_back(entity);
		}
	}

	template<typename TComponent>
	bool HasComponent(Entity entity) const {
		AssertEntityIsValid(entity);
		ComponentsPool<TComponent>* pool = GetPool<TComponent>();

		if (pool == nullptr) {
			return false;
		}

		return pool->Has(entity.GetId());
	}

	template<typename TComponent, typename ...TArgs>
	void AddComponent(Entity entity, TArgs&&... args) {
		AssertEntityIsValid(entity);
		ComponentsPool<TComponent>* pool = GetOrCreatePool<TComponent>();
		pool->Emplace(entity, std::forward<TArgs>(args)...);
	}

	template<typename TComponent>
	CH<TComponent> GetComponent(Entity entity) {
		AssertEntityIsValid(entity);
		ComponentsPool<TComponent>* pool = GetPool<TComponent>();
		return { pool, entity.GetId() };
	}

	template<typename TComponent>
	void RemoveComponent(Entity entity) {
		AssertEntityIsValid(entity);
		ComponentsPool<TComponent>* pool = GetPool<TComponent>();

		if (pool != nullptr) {
			pool->Remove(entity.GetId());
		}
	}

	void Validate() {
		for (Entity entity : to_destroy_) {
			if (!IsEntityValid(entity)) {
				continue;
			}

			for (auto& it : component_pools_) {
				it.second->Remove(entity.GetId());
			}

			EntityDestroyed.Notify(entity);
			entities_pool_.Destroy(entity);
		}
	}

	template <typename ...TComponents, typename TFunction>
	void ForEach(TFunction function) {
		ComponentsPools<TComponents...> pools = GetPools<TComponents...>();

		if (HasNullPools(pools)) {
			return;
		}

		for (Entity entity : entities_pool_) {
			if (!HasRequiredComponents(pools, entity)) {
				continue;
			}

			function(entity, CH<TComponents>(std::get<ComponentsPool<TComponents>*>(pools), entity)...);
		}
	}

	template <typename ...TComponents>
	Filter<TComponents...> FilterEntities() {
		return Filter<TComponents...>(this);
	}

private:
	std::unordered_map<TypeId, IComponentsPool*> component_pools_;
	EntitiesPool entities_pool_;
	std::vector<Entity> to_destroy_;

	template<typename ...TComponents>
	static bool HasNullPools(ComponentsPools<TComponents...>& pools) {
		return ((std::get<ComponentsPool<TComponents>*>(pools) == nullptr) || ...);
	}

	template<typename ...TComponents>
	static bool HasRequiredComponents(ComponentsPools<TComponents...>& pools, Entity entity) {
		return (std::get<ComponentsPool<TComponents>*>(pools)->Has(entity) && ...);
	}

	void AssertEntityIsValid(Entity entity) const {
		AOE_ASSERT_MSG(IsEntityValid(entity), "Invalid entity.");
	}

	template<typename TComponent>
	ComponentsPool<TComponent>* GetPool() const {
		TypeId type_id = Identifier::GetTypeId<TComponent>();
		auto it = component_pools_.find(type_id);

		if (it == component_pools_.end()) {
			return nullptr;
		}

		return static_cast<ComponentsPool<TComponent>*>(it->second);
	}

	template<typename ...TComponents>
	ComponentsPools<TComponents...> GetPools() const {
		return std::make_tuple(GetPool<TComponents>()...);
	}

	template<typename TComponent>
	ComponentsPool<TComponent>* CreatePool() {
		TypeId type_id = Identifier::GetTypeId<TComponent>();
		ComponentsPool<TComponent>* pool = new ComponentsPool<TComponent>();
		component_pools_[type_id] = pool;
		return pool;
	}

	template<typename TComponent>
	ComponentsPool<TComponent>* GetOrCreatePool() {
		ComponentsPool<TComponent>* pool = GetPool<TComponent>();

		if (pool == nullptr) {
			pool = CreatePool<TComponent>();
		}

		return pool;
	}
};

} // namespace aoe