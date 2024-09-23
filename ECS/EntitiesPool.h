#pragma once

#include <vector>

#include "Entity.h"

namespace aoe {

class EntitiesPool {
private:
	using InnerIterator = std::vector<Entity>::iterator;

public:
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = Entity;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator(InnerIterator it)
			: it_(it)
		{}

		reference operator*() {
			return *it_;
		}

		pointer operator->() {
			return &(*it_);
		}

		Iterator& operator++() {
			std::advance(it_, 1);
			return *this;
		}

		Iterator operator++(int) {
			Iterator temp = *this;
			std::advance(it_, 1);
			return temp;
		}

		friend bool operator==(const Iterator& lhs, const Iterator& rhs) {
			return lhs.it_ == rhs.it_;
		}

		friend bool operator!=(const Iterator lhs, const Iterator& rhs) {
			return lhs.it_ != rhs.it_;
		}

	private:
		InnerIterator it_;
	};

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
		auto it = dense_.begin();
		return { it };
	}

	Iterator end() {
		auto it = std::next(dense_.begin(), bound_);
		return { it };
	}

private:
	using Lookup = EntityId;

	std::vector<Lookup> sparse_;
	std::vector<Entity> dense_;

	Lookup bound_;
};

} // namespace aoe