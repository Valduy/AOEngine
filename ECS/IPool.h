#pragma once

#include "ECS.h"

namespace aoe {

class IPool {
public:
	virtual bool Has(EntityId entity_id) = 0;
	virtual void Remove(EntityId entity_id) = 0;
	virtual ~IPool() = default;
};

} // namespace aoe