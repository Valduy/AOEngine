#pragma once

#include "../Reflection/Reflection.h"

namespace aoe {

class IReflectable {
AOE_REFLECTION_BEGIN(IReflectable)
AOE_REFLECTION_END()

public:
	virtual const Type* GetType() = 0;
	virtual ~IReflectable() {}
};

} // namespace aoe