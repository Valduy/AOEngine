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
		return dense_[lookup].data;
	}

	const TData& Get(Id id) const {
		AOE_ASSERT_MSG(Has(id), "Try get no existing id.");

		Lookup lookup = sparse_[id];
		return dense_[lookup].data;
	}

	template<typename ...TArgs>
	void Emplace(Id id, TArgs&&... args) {
		AOE_ASSERT_MSG(!Has(id), "Try to add an already existing id.");
		AOE_ASSERT_MSG(bound_ <= dense_.size(), "Invalid dense bound.");

		if (sparse_.size() <= id) {
			sparse_.resize(id + 1, kUndefined);
		}

		if (bound_ < dense_.size()) {
			dense_[bound_] = { id, std::forward<TArgs>(args)... };
		}
		else {
			dense_.emplace_back(id, std::forward<TArgs>(args)...);
		}

		sparse_[id] = bound_;
		bound_ += 1;
	}

	void Add(Id id, const TData& data) {
		Emplace(id, data);
	}

	void Add(Id id, TData&& data) {
		Emplace(id, std::move(data));
	}

	void Remove(Id id) {
		if (!Has(id)) {
			return;
		}

		bound_ -= 1;

		Lookup lookup = sparse_[id];
		Id moved = dense_[bound_].id;

		std::swap(dense_[lookup], dense_[bound_]);

		sparse_[moved] = lookup;
		sparse_[id] = kUndefined;
	}

private:
	struct Node {
		Id id;
		TData data;

		template<typename ...TArgs>
		Node(Id id, TArgs&&... args)
			: id(id)
			, data(std::forward<TArgs>(args)...)
		{}
	};

	using Lookup = int32_t;

	static const Lookup kUndefined = -1;

	std::vector<Lookup> sparse_;
	std::vector<Node> dense_;

	Lookup bound_;
};

} // namespace aoe