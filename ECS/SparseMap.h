#pragma once

#include <vector>

#include "../Core/Debug.h"

namespace aoe {

template<typename TData>
class SparseMap {
private:
	template<typename TValue>
	using InnerIterator = std::vector<TValue>::iterator;

public:
	using Id = size_t;
	
	class Iterator {
	public:
		using iterator_category = std::forward_iterator_tag;
		using difference_type = std::ptrdiff_t;
		using value_type = std::pair<const Id, TData&>;
		using pointer = value_type*;
		using reference = value_type&;

		Iterator(InnerIterator<std::pair<Id, TData>> it)
			: it_(it)
			, value_(it_->first, it_->second)
		{}

		value_type operator*() const {
			return value_;
		}

		pointer operator->() {
			return &value_;
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
		InnerIterator<std::pair<Id, TData>> it_;
		value_type value_;
	};

	SparseMap()
		: sparse_()
		, dense_()
		, bound_(0)
	{}

	bool Has(Id id) const {
		if (id >= sparse_.size()) {
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
		Id other_id = dense_[bound_].first;

		std::swap(dense_[lookup], dense_[bound_]);

		sparse_[other_id] = lookup;
		sparse_[id] = kUndefined;
	}

	Iterator begin() {
		return { dense_.begin() };
	}

	Iterator end() {
		auto end = dense_.begin();
		std::advance(end, bound_);
		return { end };
	}

private:
	using Lookup = int32_t;

	static const Lookup kUndefined = -1;

	std::vector<Lookup> sparse_;
	std::vector<std::pair<Id, TData>> dense_;

	Lookup bound_;
};

} // namespace aoe