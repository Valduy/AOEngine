#pragma once

#include "ECS.h"

namespace aoe {

class Entity {
public:
	static const Version kInitialVersion = 0;

	Version GetVersion() const {
		return version_;
	}

	EntityId GetId() const {
		return id_;
	}
	
	Entity(EntityId id)
		: Entity(kInitialVersion, id)
	{}

	Entity(Version version, EntityId id)
		: version_(version)
		, id_(id)
	{}

	friend bool operator==(const Entity& lhs, const Entity& rhs) {
		return lhs.version_ == rhs.version_ && lhs.id_ == rhs.id_;
	}

private:
	Version version_;
	EntityId id_;
};

} // namespace aoe