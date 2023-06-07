#pragma once

#include "IReflectable.h"

namespace aoe {

template<typename TComponent>
class Component : public IReflectable {
AOE_REFLECTION_BEGIN(Component<TComponent>)
AOE_REFLECTION_END()

public:
	const Type* GetType() override {
		static const Type* type = Reflector::GetType<TComponent>();
		return type;
	}
};

} // namespace aoe