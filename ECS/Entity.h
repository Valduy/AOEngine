#pragma once

#include "ECS.h"

namespace aoe {

class Entity {
public:
	static const Version kInitialVersion = 0;

	Entity(EntityId id)
		: Entity(kInitialVersion, id)
	{}

	Entity(Version version, EntityId id)
		: version_(version)
		, id_(id)
	{
		AOE_ASSERT_MSG(version_ >= kNullVersion, "Invalid version.");
		AOE_ASSERT_MSG(id_ >= kNullEntityId, "Invalid entity id.");
	}

	static Entity Null() {
		return { kNullVersion, kNullEntityId };
	}

	bool IsNull() const {
		return version_ == kNullVersion && id_ == kNullEntityId;
	}

	Version GetVersion() const {
		return version_;
	}

	EntityId GetId() const {
		return id_;
	}
	
	friend bool operator==(const Entity& lhs, const Entity& rhs) {
		return lhs.version_ == rhs.version_ && lhs.id_ == rhs.id_;
	}

private:
	Version version_;
	EntityId id_;
};

} // namespace aoe

namespace std {

	template<>
	struct hash<aoe::Entity> {
		size_t operator()(const aoe::Entity& value) const noexcept {
			size_t hash0 = hash<aoe::Version>()(value.GetVersion());
			size_t hash1 = hash<aoe::EntityId>()(value.GetId());
			return hash0 ^ hash1;
		}
	};

} // namespace std