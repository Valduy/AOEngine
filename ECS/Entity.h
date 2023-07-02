#pragma once

#include <new>
#include <vector>
#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>

#include "IReflectable.h"

namespace aoe {

class Entity {
private:
	using LookupIndex = int32_t;

public:
	template<typename TComponent>
	class ComponentHandler {
		friend class Entity;

	public:
		bool IsValid() const {
			if (lookup_index_ == kInvalidIndex || source_->table_.GetSize() <= lookup_index_) {
				return false;
			}

			Lookup lookup = source_->table_[lookup_index_];

			if (lookup.type_id_ != type_id_) {
				return false;
			}

			return true;
		}

		TComponent* operator->() const {
			if (!IsValid()) {
				assert(false && "Handler is invalid.");
				return nullptr;
			}

			Lookup lookup = source_->table_[lookup_index_];
			NodeHandler node(source_, lookup.offset_);
			return reinterpret_cast<TComponent*>(GetComponent(node));
		}

	private:
		Entity* source_;
		const TypeId type_id_;
		const LookupIndex lookup_index_;

		ComponentHandler(
			Entity* source,
			TypeId type_id,
			LookupIndex lookup_index)
			: source_(source)
			, type_id_(type_id)
			, lookup_index_(lookup_index)
		{}
	};

	Entity()
		: root_(this, kInvalidOffset)
		, buffer_(nullptr)
		, size_(0)
		, capacity_(0)
		, table_()
	{}

	~Entity() {
		// TODO: go throw all components and call destructor manually;
		// free buffer;
	}

	template<typename TComponent>
	ComponentHandler<TComponent> Add() {
		TypeId key = Identifier::GetTypeId<TComponent>();
		NodeHandler node = GetNode(key, sizeof(TComponent));
		IReflectable* component = SetComponent<TComponent>(node);
		return { this, key, node.GetLookupIndex() };
	}

	template<typename TComponent>
	ComponentHandler<TComponent> Get() {
		TypeId key = Identifier::GetTypeId<TComponent>();
		NodeHandler temp = root_;

		while (temp.IsValid()) {
			auto test = temp.GetKey();

			if (temp.GetKey() > key) {
				temp = temp.GetLeft();
				continue;
			}
			
			if (temp.GetKey() < key) {
				temp = temp.GetRight();
				continue;
			}

			return { this, key, temp.GetLookupIndex() };
		}

		return { this, Identifier::kInvalid, kInvalidIndex };
	}

	template<typename TComponent>
	void Remove() {
		// TODO:
	}

	void DebugPrint() {
		if (root_.IsValid()) {
			PrintTree(root_, "", true);
		}	
	}

private:
	using Offset = int32_t;
	using Height = char8_t;

	struct Node {
		TypeId key;
		size_t size;
		Height height;
		LookupIndex lookup_index;

		Offset parent_offset;
		Offset left_offset;
		Offset right_offset;

		Node()
			: Node(Identifier::kInvalid, 0, kInvalidIndex)
		{}

		Node(TypeId key, Offset size, LookupIndex lookup_index)
			: key(key)
			, size(size)
			, height(0)
			, lookup_index(lookup_index)
			, parent_offset(kInvalidOffset)
			, left_offset(kInvalidOffset)
			, right_offset(kInvalidOffset)
		{}
	};
	
	class NodeHandler {
	public:
		bool IsValid() const {
			return offset_ != kInvalidOffset;
		}

		Offset GetOffset() const {
			return offset_;
		}

		void* GetAddress() const {
			return reinterpret_cast<char*>(owner_->buffer_) + offset_;
		}

		TypeId GetKey() const {
			return (*this)->key;
		}

		LookupIndex GetLookupIndex() const {
			return (*this)->lookup_index;
		}

		NodeHandler GetParent() const {
			return { owner_, (*this)->parent_offset };
		}

		void SetParent(NodeHandler node) {
			(*this)->parent_offset = node.GetOffset();
		}

		NodeHandler GetLeft() const {
			return { owner_, (*this)->left_offset };
		}

		void SetLeft(NodeHandler node) {
			(*this)->left_offset = node.GetOffset();
		}

		NodeHandler GetRight() const {
			return { owner_, (*this)->right_offset };
		}

		void SetRight(NodeHandler node) {
			(*this)->right_offset = node.GetOffset();
		}

		NodeHandler(Entity* owner, Offset offset)
			: owner_(owner)
			, offset_(offset)
		{}

		Node* operator->() const {
			return reinterpret_cast<Node*>(GetAddress());
		}
		
	private:
		Entity* owner_;
		Offset offset_;
	};

	struct Lookup {
		TypeId type_id_{ Identifier::kInvalid };
		Offset offset_{ kInvalidOffset};
	};

	class LookupTable {
	public:
		size_t GetSize() {
			return table_.size();
		}
		
		LookupTable()
			: table_()
			, head_(kInvalidIndex)
		{}

		LookupIndex CreateLookup(TypeId type_id, Offset offset) {
			if (head_ == kInvalidIndex) {
				Lookup& lookup = table_.emplace_back(type_id, offset);
				return table_.size() - 1;
			} else {
				int lookup_index = head_;
				Lookup& lookup = table_[lookup_index];
				head_ = lookup.offset_;
				lookup.type_id_ = type_id;
				lookup.offset_ = offset;
				return lookup_index;
			}			
		}

		void DeleteLookup(LookupIndex lookup_index) {
			assert(lookup_index >= 0 && lookup_index < table_.size() && "Invalid lookup index.");
			Lookup& lookup = table_[lookup_index];
			lookup.type_id_ = Identifier::kInvalid;
			lookup.offset_ = head_;
			head_ = lookup_index;
		}

		Lookup& operator[](LookupIndex index) {
			return table_[index];
		}

	private:
		std::vector<Lookup> table_;
		LookupIndex head_;
	};

	friend bool operator==(const NodeHandler& lhs, const NodeHandler& rhs) {
		return lhs.GetOffset() == rhs.GetOffset();
	}

	static const Offset kInvalidOffset = -1;
	static const LookupIndex kInvalidIndex = -1;

	NodeHandler root_;

	void* buffer_;
	size_t size_;
	size_t capacity_;

	LookupTable table_;

	static IReflectable* GetComponent(NodeHandler node) {
		char* temp = reinterpret_cast<char*>(node.GetAddress());
		temp += sizeof(Node);
		return reinterpret_cast<IReflectable*>(temp);
	}

	template<typename TComponent>
	static TComponent* GetComponent(Node* node) {
		return static_cast<TComponent*>(GetComponent(node));
	}

	template<typename TComponent>
	static IReflectable* SetComponent(NodeHandler node) {
		char* temp = reinterpret_cast<char*>(node.GetAddress());
		temp += sizeof(Node);
		IReflectable* component = new (temp) TComponent;
		return component;
	}

	void PrintTree(NodeHandler tree, std::string indent, bool last)
	{
		std::cout << indent << "+- " << tree->key << "\n";
		indent += last ? "   " : "|  ";

		if (tree.GetLeft().IsValid())
		{
			PrintTree(tree.GetLeft(), indent, false);
		}
		if (tree.GetRight().IsValid())
		{
			PrintTree(tree.GetRight(), indent, true);
		}
	}

	NodeHandler GetNode(TypeId key, size_t component_size) {
		if (!root_.IsValid()) {
			root_ = CreateNewNode(key, component_size);
			return root_;
		}

		NodeHandler node = root_;

		while (true) {
			if (key < node->key) {
				if (!node.GetLeft().IsValid()) {
					NodeHandler new_node = CreateNewNode(key, component_size);
					new_node.SetParent(node);
					node.SetLeft(new_node);
					Balance(new_node);
					return new_node;
				}

				node = node.GetLeft();
				continue;
			}
			if (key > node->key) {
				if (!node.GetRight().IsValid()) {
					NodeHandler new_node = CreateNewNode(key, component_size);
					new_node.SetParent(node);
					node.SetRight(new_node);
					Balance(new_node);
					return new_node;
				}

				node = node.GetRight();
				continue;
			}

			// key == node->key
			IReflectable* component = GetComponent(node);
			component->~IReflectable();
			return node;
		}
	}

	NodeHandler CreateNewNode(TypeId key, size_t component_size) {
		size_t available = capacity_ - size_;
		size_t desired = sizeof(Node) + component_size;

		if (available < desired) {			
			void* temp = buffer_;
			capacity_ = size_ + desired; // TODO: amortization
			buffer_ = malloc(capacity_);
			memmove(buffer_, temp, size_);
			free(temp);			
		}
		
		Offset offset = size_;
		void* address = reinterpret_cast<char*>(buffer_) + offset;
		LookupIndex lookup_index = table_.CreateLookup(key, offset);
		new (address) Node(key, component_size, lookup_index);
		size_ += desired;
		return { this, offset };
	}

	void Balance(NodeHandler temp) {
		while (temp.IsValid()) {
			NodeHandler parent = temp.GetParent();
			FixHeight(temp);
			int balance_factor = BalanceFactor(temp);

			if (balance_factor == 2) {
				NodeHandler right = temp.GetRight();

				if (BalanceFactor(right) < 0) {
					temp.SetRight(RotateRight(right));
				}
				
				if (!parent.IsValid()) {
					root_ = RotateLeft(temp);
				} else if (parent.GetLeft() == temp) {
					parent.SetLeft(RotateLeft(temp));
				} else {
					parent.SetRight(RotateLeft(temp));
				}
			} else if (balance_factor == -2) {
				NodeHandler left = temp.GetLeft();

				if (BalanceFactor(left) > 0) {
					temp.SetLeft(RotateLeft(left));
				}

				if (!parent.IsValid()) {
					root_ = RotateRight(temp);
				} else if (parent.GetLeft() == temp) {
					parent.SetLeft(RotateRight(temp));
				} else {
					parent.SetRight(RotateRight(temp));
				}
			}

			temp = temp.GetParent();
		}
	}

	int BalanceFactor(NodeHandler node) {
		return GetHeight(node.GetRight()) - GetHeight(node.GetLeft());
	}

	Height GetHeight(NodeHandler node) {
		return node.IsValid() ? node->height : 0;
	}

	NodeHandler RotateRight(NodeHandler node) {
		NodeHandler left = node.GetLeft();
		NodeHandler left_right = left.GetRight();

		NodeHandler temp = node.GetLeft();
		left.SetParent(node.GetParent());
		node.SetLeft(left.GetRight());

		if (left_right.IsValid()) {
			left_right.SetParent(node);
		}
		
		left.SetRight(node);
		node.SetParent(temp);

		FixHeight(node);
		FixHeight(left);
		return temp;
	}

	NodeHandler RotateLeft(NodeHandler node) {
		NodeHandler right = node.GetRight();
		NodeHandler right_left = right.GetLeft();

		NodeHandler temp = node.GetRight();
		right.SetParent(node.GetParent());
		node.SetRight(right.GetLeft());

		if (right_left.IsValid()) {
			right_left.SetParent(node);
		}
		
		right.SetLeft(node);
		node.SetParent(temp);

		FixHeight(node);
		FixHeight(right);
		return temp;
	}

	void FixHeight(NodeHandler node) {
		Height left_height = GetHeight(node.GetLeft());
		Height right_height = GetHeight(node.GetRight());
		node->height = left_height > right_height ? left_height : right_height;
		node->height += 1;
	}
};

} // namespace aoe