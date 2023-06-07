#pragma once

#include <new>
#include <vector>
#include <algorithm>

#include "IReflectable.h"

namespace aoe {

// maybe trivial identifier is better solution

class Entity {
public:

	Entity()
		: lookups_()
		, buffer_(nullptr)
		, size_(0)
		, capacity_(0)
	{}

	~Entity() {
		// TODO: go throw all components and call destructor manually;
		// free buffer;
	}

	template<typename TComponent>
	TComponent& Add() {
		size_t available = capacity_ - size_;
		size_t desired = sizeof(Block) + sizeof(TComponent);

		if (available < desired) {			
			void* temp = buffer_;
			buffer_ = malloc(size_ + desired);
			capacity_ = size_ + desired;
			memmove(buffer_, temp, size_);
			free(temp);			
		}

		void* address = buffer_ + size_;
		new (address) Block{sizeof(TComponent)};
		TComponent* component = new (address + sizeof(Block)) TComponent;

		// TODO: check if already exist
		InsertLookup(Identifier::GetTypeId<TComponent>(), address);
		return *component;
	}

	template<typename TComponent>
	void Remove() {

	}

	template<typename TComponent>
	TComponent& Get() {
		TypeId type_id = Identifier::GetTypeId<TComponent>();

	}

private:
	struct Block {
		size_t size;
	};

	struct Lookup {
		TypeId component_id;
		Block* address;
	};

	std::vector<Lookup> lookups_;
	void* buffer_;
	size_t size_;
	size_t capacity_;

	static bool Less(const Lookup& lhs, const Lookup& rhs) {
		return lhs.component_id < rhs.component_id;
	}

	void InsertLookup(TypeId type_id, Block* address) {
		Lookup lookup{ type_id, address };
		auto less_it = std::upper_bound(lookups_.begin(), lookups_.end(), lookup, Less);
		lookups_.insert(less_it, lookup);
	}

	const Lookup* GetLookup(TypeId type_id) {
		auto it = std::find(lookups_.begin(), lookups_.end(), Less);
		//return it == lookups_.end() ? nullptr : it
	}

	//IComponent* table_[kMaxComponentsCount];

	//void* buffer_;
	//void* temp_;

	//size_t buffer_size_;
	// some buffer


};

} // namespace aoe