#pragma once

#include "../Reflection/Reflection.h"

namespace aoe {

class IComponent {
AOE_REFLECTION_BEGIN(IComponent)
AOE_REFLECTION_END()

public:
	virtual const Type* GetType() = 0;
	virtual ~IComponent() {}
};

} // namespace aoe