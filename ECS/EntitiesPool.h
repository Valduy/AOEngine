#pragma once

#include <vector>

#include "Entity.h"

namespace aoe {

class EntitiesPool {
public:
	using Iterator = std::vector<Entity>::iterator;
	using ConstIterator = std::vector<Entity>::const_iterator;

	EntitiesPool()
		: sparse_()
		, dense_()
		, bound_(0)
	{}

	bool IsValid(Entity entity) const {
		if (entity.GetId() < 0 || entity.GetId() >= sparse_.size()) {
			return false;
		}

		Lookup lookup = sparse_[entity.GetId()];

		if (lookup >= bound_) {
			return false;
		}

		return dense_[lookup].GetVersion() == entity.GetVersion();
	}

	Entity Create() {
		AOE_ASSERT_MSG(bound_ <= dense_.size(), "Invalid dense bound.");

		Entity entity = Entity::Null();

		if (bound_ < dense_.size()) {
			entity = dense_[bound_];
		} else {
			sparse_.push_back(bound_);
			entity = dense_.emplace_back(bound_);
		}

		bound_ += 1;
		return entity;
	}

	void Destroy(Entity entity) {
		bound_ -= 1;
		Entity moved = dense_[bound_];

		std::swap(sparse_[entity.GetId()], sparse_[moved.GetId()]);

		dense_[sparse_[moved.GetId()]] = moved;
		dense_[sparse_[entity.GetId()]] = { entity.GetVersion() + 1, entity.GetId() };
	}

	Iterator begin() {
		return dense_.begin();
	}

	Iterator end() {
		return std::next(dense_.begin(), bound_);
	}

	ConstIterator cbegin() const {
		return dense_.cbegin();
	}

	ConstIterator cend() const {
		return std::next(dense_.cbegin(), bound_);
	}

private:
	using Lookup = EntityId;

	std::vector<Lookup> sparse_;
	std::vector<Entity> dense_;

	Lookup bound_;
};

} // namespace aoe