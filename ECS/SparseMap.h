#pragma once

#include <vector>

#include "../Core/Debug.h"

namespace aoe {

template<typename TData>
class SparseMap {
public:
	using Id = size_t;

	SparseMap()
		: sparse_()
		, dense_()
		, bound_(0)
	{}

	bool Has(Id id) const {
		if (id < 0 || id >= sparse_.size()) {
			return false;
		}

		Lookup lookup = sparse_[id];
		return lookup != kUndefined;
	}

	TData& Get(Id id) {
		AOE_ASSERT_MSG(Has(id), "Try get no existing id.");

		Lookup lookup = sparse_[id];
		return dense_[lookup].second;
	}

	template<typename ...TArgs>
	void Add(Id id, TArgs&&... args) {
		TData data(std::forward<TArgs>(args)...);
		Add(id, std::move(data));
	}

	void Add(Id id, TData&& data) {
		AOE_ASSERT_MSG(!Has(id), "Try to add an already existing id.");
		AOE_ASSERT_MSG(bound_ <= dense_.size(), "Invalid dense bound.");
		
		if (sparse_.size() <= id) {
			sparse_.resize(id + 1, kUndefined);
		}

		if (bound_ < dense_.size()) {
			std::pair<Id, TData>& pair = dense_[bound_];
			pair.first = id;
			pair.second = std::move(data);
		} else {
			dense_.emplace_back(id, std::forward<TData>(data));
		}

		sparse_[id] = bound_;
		bound_ += 1;
	}

	void Remove(Id id) {
		if (!Has(id)) {
			return;
		}

		bound_ -= 1;

		Lookup lookup = sparse_[id];
		Id moved = dense_[bound_].first;

		std::swap(dense_[lookup], dense_[bound_]);

		sparse_[moved] = lookup;
		sparse_[id] = kUndefined;
	}

private:
	using Lookup = int32_t;

	static const Lookup kUndefined = -1;

	std::vector<Lookup> sparse_;
	std::vector<std::pair<Id, TData>> dense_;

	Lookup bound_;
};

} // namespace aoe