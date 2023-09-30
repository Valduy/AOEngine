#pragma once

#include "Entity.h"

namespace aoe {

using EntityId = size_t;

class World {
	Entity& Create() {
		// TODO: implementation
	}

	void Destroy(Entity& entity) {
		// TODO: implementation
	}

	// iteration over all entities

private:
	std::vector<Entity> pools_;
	std::vector<EntityId> free_;
	std::vector<EntityId> marked_to_delete_;
};

} // namespace aoe