#pragma once

#include "../Core/Debug.h"

#include "ECS.h"

namespace aoe {

template<typename T>
class Pool;

template<typename TComponent>
class ComponentHandler {
public:
	ComponentHandler()
		: ComponentHandler(nullptr, kNullEntity)
	{}

	ComponentHandler(Pool<TComponent>* pool, EntityId entity_id)
		: pool_(pool)
		, entity_id_(entity_id)
	{}

	bool IsValid() const {
		if (pool_ == nullptr) {
			return false;
		}

		TComponent* component = pool_->Get(entity_id_);
		return component != nullptr;
	}

	TComponent* Get() const {
		AOE_ASSERT_MSG(pool_ != nullptr, "Handler is invalid.");
		TComponent* component = pool_->Get(entity_id_);
		AOE_ASSERT_MSG(component != nullptr, "Handler is invalid.");
		return component;
	}

	TComponent* operator->() const {
		return Get();
	}

private:
	Pool<TComponent>* pool_;
	EntityId entity_id_;
};

} // namespace aoe