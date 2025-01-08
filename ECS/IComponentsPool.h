#pragma once

#include "Entity.h"

namespace aoe {

class IComponentsPool {
public:
	virtual ~IComponentsPool() = default;
	virtual bool Has(Entity entity) const = 0;
	virtual void Remove(Entity entity) = 0;
};

} // namespace aoe