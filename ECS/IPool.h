#pragma once

#include "Entity.h"

namespace aoe {

class IPool {
public:
	virtual ~IPool() = default;
	virtual bool Has(Entity entity) = 0;
	virtual void Remove(Entity entity) = 0;
};

} // namespace aoe