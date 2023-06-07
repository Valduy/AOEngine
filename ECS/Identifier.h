#pragma once

#include <cassert>

namespace aoe {

class Identifier {
public:
	using Id = size_t;

	static constexpr Id kOverflowId = 0;

	template<typename TComponent>
	static Id GetId() {
		static Id component_id = GetNextId();
		assert(component_id != kOverflowId && "Component's id overflow.");
		return component_id;
	}

private:
	static Id current_component_id_;

	Identifier() = default;

	static Id GetNextId() {
		const Id component_id = current_component_id_;
		current_component_id_ += 1;
		return component_id;
	}
};

}