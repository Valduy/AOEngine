#pragma once

#include "ECS.h"

namespace aoe {

struct Entity {
	static const Version kInitialVersion = 0;

	const Version version;
	const EntityId id;
	
	Entity(EntityId id)
		: Entity(kInitialVersion, id)
	{}

	Entity(Version version, EntityId id)
		: version(version)
		, id(id)
	{}

	friend bool operator==(const Entity& lhs, const Entity& rhs) {
		return lhs.version == rhs.version && lhs.id == rhs.id;
	}
};

} // namespace aoe