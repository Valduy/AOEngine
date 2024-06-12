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
		: ComponentHandler(nullptr, Entity::Null())
	{}

	ComponentHandler(Pool<TComponent>* pool, Entity entity)
		: pool_(pool)
		, entity_(entity)
	{}

	bool IsValid() const {
		if (pool_ == nullptr || entity_.IsNull()) {
			return false;
		}

		TComponent* component = pool_->Get(entity_);
		return component != nullptr;
	}

	TComponent* Get() const {
		AOE_ASSERT_MSG(pool_ != nullptr || entity_.IsNull(), "Handler is invalid.");
		TComponent* component = pool_->Get(entity_);
		AOE_ASSERT_MSG(component != nullptr, "Entity doesn't have a required component.");
		return component;
	}

	TComponent* operator->() const {
		return Get();
	}

private:
	Pool<TComponent>* pool_;
	Entity entity_;
};

} // namespace aoe